#include "ChunkGeneration.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <deque>
#include <exception>
#include <limits>
#include <map>
#include <mutex>
#include <set>
#include <stdexcept>
#include <thread>
#include <tuple>

namespace WorldMaker::ChunkGeneration {

ChunkRenderUnit* GetChunkAtPos(ChunkArray& chunks, glm::ivec2 pos)
{
	const auto& res = std::find_if(chunks.cbegin(), chunks.cend(), [pos](const std::pair<glm::ivec2, ChunkRenderUnit*>& ck) { return ck.first == pos; });
	if (res != chunks.cend()) {
		return res->second;
	}

	return nullptr;
}

GenerationSettings::GenerationSettings(const WorldGenerator& source)
    : generator(source), biomes(BiomeGenerator::m_biomes), features(FeatureManager::m_features),
      resolution(ChunkRenderUnit::s_chunkRes), scale(ChunkRenderUnit::s_chunkScale) {}

bool GenerateChunkData(const GenerationSettings& settings, ChunkRequest request,
                       ChunkData& data, const std::atomic_bool& cancelled) {
    if (cancelled.load()) return false;
    if (request.lod < 0 || request.lod > ChunkRenderUnit::MAX_LOD ||
        settings.resolution < 16 || settings.resolution % 16 != 0 || settings.scale <= 0 || settings.biomes.empty())
        throw std::invalid_argument("Invalid chunk generation settings");
    data = {};
    data.request = request;
    data.tallest = std::numeric_limits<double>::lowest();
    data.lowest = std::numeric_limits<double>::max();
	data.id = (request.position.x << 16) | (request.position.y & 0xff);
    // Noise objects are private to this job, even though sampling is read-only.
    auto generator = settings.generator;
    const int cells = settings.resolution >> request.lod;
    const int side = cells + 1;
    const int step = 1 << request.lod;
    const auto sample = [&](int x, int z) {
        const float worldX = (float(request.position.x * cells + x) * step / settings.resolution) * settings.scale;
        const float worldZ = (float(request.position.y * cells + z) * step / settings.resolution) * settings.scale;
        return generator.getVertex(worldX, worldZ, settings.biomes, settings.features);
    };

    data.vertices.reserve(side * side);
    for (int z = 0; z < side; ++z) {
        if (cancelled.load()) return false;
        for (int x = 0; x < side; ++x) {
            auto vertex = sample(x, z);
            data.tallest = std::max(data.tallest, vertex.m_position.y);
            data.lowest = std::min(data.lowest, vertex.m_position.y);
            if (vertex.m_featureId != 0) {
                const auto& feature = settings.features[vertex.m_featureId - 1];
                data.models.push_back({feature.modelPath,
                    {vertex.m_position.x, vertex.m_position.y + feature.y_offset, vertex.m_position.z},
                    feature.rotation, feature.scale});
            }
            data.vertices.push_back(vertex.toGraphicalVertex());
        }
    }

    data.indices.reserve(cells * cells * 6);
    for (int z = 0; z < side; ++z) {
        if (cancelled.load()) return false;
        for (int x = 0; x < side; ++x) {
            const auto height = [&](int sx, int sz) {
                if (sx < 0 || sz < 0 || sx >= side || sz >= side) return sample(sx, sz).m_position.y;
                return data.vertices[sz * side + sx].m_position.y;
            };
            data.vertices[z * side + x].m_normal = glm::normalize(glm::vec3(
                height(x - 1, z) - height(x + 1, z), 2.0f, height(x, z + 1) - height(x, z - 1)));
            if (x < cells && z < cells) {
                const unsigned int bottomLeft = z * side + x;
                const unsigned int topLeft = (z + 1) * side + x;
                data.indices.insert(data.indices.end(),
                    {bottomLeft, topLeft + 1, bottomLeft + 1, bottomLeft, topLeft, topLeft + 1});
            }
        }
    }
    return !cancelled.load();
}

namespace {
using Key = std::tuple<int, int, int>;
Key key(ChunkRequest request) { return {request.position.x, request.position.y, request.lod}; }

int desiredLOD(glm::ivec2 position, glm::ivec2 camera, int distance) {
    auto delta = glm::abs(position - camera);
    const float ratio = float(std::max(delta.x, delta.y)) / distance;
    if (ratio > 0.80f) return 4;
    if (ratio > 0.60f) return 3;
    if (ratio > 0.45f) return 2;
    return 0;
}
}

struct ChunkWorkerPool::Impl {
    struct Job {
        ChunkRequest request;
        std::shared_ptr<const GenerationSettings> settings;
        std::atomic_bool cancelled = false;
        bool started = false; // Protected by mutex.
    };
    struct Result {
        std::shared_ptr<Job> job;
        ChunkData data;
        std::exception_ptr error;
    };
    std::mutex mutex;
    std::condition_variable changed;
    bool stopping = false;
    std::map<Key, std::shared_ptr<Job>> pending;
    std::deque<std::shared_ptr<Job>> queued;
    std::deque<Result> ready;
    std::vector<std::thread> workers;

    void run() {
        for (;;) {
            std::shared_ptr<Job> job;
            {
                std::unique_lock lock(mutex);
                // Bound completed mesh memory if rendering/uploading falls behind.
                changed.wait(lock, [&] { return stopping || (!queued.empty() && ready.size() < 8); });
                if (stopping) return;
                job = queued.front();
                queued.pop_front();
                job->started = true;
            }
            Result result;
            result.job = job;
            bool complete = false;
            try {
                complete = GenerateChunkData(*job->settings, job->request, result.data, job->cancelled);
            } catch (...) {
                result.error = std::current_exception();
                complete = true;
            }
            {
                std::lock_guard lock(mutex);
                if (complete && !job->cancelled && !stopping) ready.push_back(std::move(result));
            }
        }
    }
};

ChunkWorkerPool::ChunkWorkerPool(unsigned int workerCount) : m_impl(std::make_unique<Impl>()) {
    if (workerCount == 0) {
        const auto cores = std::thread::hardware_concurrency();
        workerCount = std::clamp(cores > 1 ? cores - 1 : 1u, 1u, 4u);
    }
    try {
        for (unsigned int i = 0; i < workerCount; ++i) m_impl->workers.emplace_back([this] { m_impl->run(); });
    } catch (...) {
        shutdown();
        throw;
    }
}

ChunkWorkerPool::~ChunkWorkerPool() { shutdown(); }

void ChunkWorkerPool::reset() {
    std::lock_guard lock(m_impl->mutex);
    for (auto& [key, job] : m_impl->pending) job->cancelled = true;
    m_impl->pending.clear();
    m_impl->queued.clear();
    m_impl->ready.clear();
    m_impl->changed.notify_all();
}

void ChunkWorkerPool::shutdown() {
    {
        std::lock_guard lock(m_impl->mutex);
        m_impl->stopping = true;
        for (auto& [key, job] : m_impl->pending) job->cancelled = true;
    }
    m_impl->changed.notify_all();
    for (auto& worker : m_impl->workers) if (worker.joinable()) worker.join();
    reset();
}

void ChunkWorkerPool::request(const std::vector<ChunkRequest>& requests,
                              std::shared_ptr<const GenerationSettings> settings) {
    std::lock_guard lock(m_impl->mutex);
    if (m_impl->stopping) return;
    std::set<Key> wanted;
    for (const auto& request : requests) wanted.insert(key(request));
    for (auto it = m_impl->pending.begin(); it != m_impl->pending.end();) {
        if (!wanted.contains(it->first)) {
            it->second->cancelled = true;
            it = m_impl->pending.erase(it);
        } else ++it;
    }
    std::erase_if(m_impl->ready, [](const auto& result) { return result.job->cancelled.load(); });
    m_impl->queued.clear();
    std::set<Key> enqueued;
    for (const auto& request : requests) {
        auto& job = m_impl->pending[key(request)];
        if (!job) {
            job = std::make_shared<Impl::Job>();
            job->request = request;
            job->settings = settings;
        }
        if (!job->started && enqueued.insert(key(request)).second) m_impl->queued.push_back(job);
    }
    m_impl->changed.notify_all();
}

bool ChunkWorkerPool::tryPop(ChunkData& data) {
    std::lock_guard lock(m_impl->mutex);
    if (m_impl->ready.empty()) return false;
    auto result = std::move(m_impl->ready.front());
    m_impl->ready.pop_front();
    m_impl->pending.erase(key(result.job->request));
    m_impl->changed.notify_all();
    if (result.error) std::rethrow_exception(result.error);
    data = std::move(result.data);
    return true;
}

glm::ivec2 GetChunkPos(glm::vec3 position) {
    return {int(std::floor(position.x / ChunkRenderUnit::s_chunkScale)),
            int(std::floor(position.z / ChunkRenderUnit::s_chunkScale))};
}

glm::ivec2 GetGenerationRange(int position, int renderDistance) {
    return {std::max(0, position - renderDistance), position + renderDistance};
}

void ChunkStreamer::update(ChunkArray& chunks, const WorldGenerator& generator, int renderDistance,
                           glm::vec3 cameraPosition, bool force) {
    renderDistance = std::max(1, renderDistance);
    const auto position = GetChunkPos(cameraPosition);
    const bool newWorld = !m_settings || force || m_lastResolution != ChunkRenderUnit::s_chunkRes ||
        m_lastScale != ChunkRenderUnit::s_chunkScale || m_lastSeed != generator.m_seed;
    if (newWorld) {
        m_settings = std::make_shared<GenerationSettings>(generator);
        m_workers.reset();
        for (auto& chunk : chunks) delete chunk.second;
        chunks.clear();
    }
    if (newWorld || position != m_lastPosition || renderDistance != m_lastDistance) {
        m_lastPosition = position;
        m_lastDistance = renderDistance;
        m_lastResolution = ChunkRenderUnit::s_chunkRes;
        m_lastScale = ChunkRenderUnit::s_chunkScale;
        m_lastSeed = generator.m_seed;
        const auto xRange = GetGenerationRange(position.x, renderDistance);
        const auto zRange = GetGenerationRange(position.y, renderDistance);
        std::erase_if(chunks, [&](const auto& chunk) {
            const auto p = chunk.first;
            if (p.x >= xRange.x && p.x < xRange.y && p.y >= zRange.x && p.y < zRange.y) return false;
            delete chunk.second;
            return true;
        });
        std::map<std::pair<int, int>, ChunkRenderUnit*> loaded;
        for (auto& chunk : chunks) loaded[{chunk.first.x, chunk.first.y}] = chunk.second;
        std::vector<ChunkRequest> requests;
        for (int x = xRange.x; x < xRange.y; ++x) {
            for (int z = zRange.x; z < zRange.y; ++z) {
                const int lod = desiredLOD({x, z}, position, renderDistance);
                auto found = loaded.find({x, z});
                if (found != loaded.end() && found->second->hasLOD(lod)) found->second->setLOD(lod);
                else requests.push_back({{x, z}, lod});
            }
        }
        std::stable_sort(requests.begin(), requests.end(), [&](const auto& a, const auto& b) {
            auto da = a.position - position;
            auto db = b.position - position;
            return double(da.x) * da.x + double(da.y) * da.y < double(db.x) * db.x + double(db.y) * db.y;
        });
        m_workers.request(requests, m_settings);
    }

    // GL and the resource cache belong to the main thread. Limit uploads per frame
    // and stop after a small time budget; an individual driver call cannot be preempted.
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(2);
    for (int uploads = 0; uploads < 2; ++uploads) {
        ChunkData data;
        if (!m_workers.tryPop(data)) break;
        auto found = std::find_if(chunks.begin(), chunks.end(), [&](const auto& chunk) {
            return chunk.first == data.request.position;
        });
        ChunkModels models;
        const bool replaceModels = found == chunks.end() || data.request.lod < found->second->m_modelsLOD;
        if (replaceModels) {
            for (const auto& instance : data.models) {
                if (!m_modelCache.contains(instance.path))
                    m_modelCache.emplace(instance.path, ResourceManager::LoadModel(instance.path));
                models.addInstance(instance.path, instance.position, instance.rotation, instance.scale, false);
            }
            models.submitData();
        }
        if (found == chunks.end()) {
            chunks.push_back({data.request.position, new ChunkRenderUnit(data.vertices, data.tallest,
                data.lowest, std::move(models), data.request.lod, data.id, &data.indices)});
        } else {
            found->second->uploadLOD(data.vertices, data.request.lod, data.tallest, data.lowest,
                replaceModels ? &models : nullptr, &data.indices);
            found->second->setLOD(data.request.lod);
        }
        if (std::chrono::steady_clock::now() >= deadline) break;
    }
}

}
