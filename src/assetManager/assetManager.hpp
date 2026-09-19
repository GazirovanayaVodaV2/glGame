#pragma once

#include <string>
#include <map>
#include <memory>
#include <utility>

#include "shader.hpp"
#include "textures/texture.hpp"
#include "assetManager/models/mesh.hpp"


/*Vibe coded*/
template <typename T>
struct assetContainerTraits {
	using keyType = std::string;
	using containerType = std::map<keyType, std::unique_ptr<T>>;
};

template <typename T>
class assetPool {
protected:
	using keyType = typename assetContainerTraits<T>::keyType;
	using containerType = typename assetContainerTraits<T>::containerType;
	static inline containerType m_resources;

	static std::string keyToString(const keyType& key) {
		if constexpr (std::is_same_v<keyType, std::string>) {
			return key;
		}
		else {
			return std::to_string(key);
		}
	}

	template <typename... Args>
	static void add(keyType key, Args&&... args) {
		if constexpr (std::is_same_v<containerType, std::vector<std::unique_ptr<T>>>) {
			if (key >= m_resources.size()) {
				m_resources.resize(key + 1);
			}
			m_resources[key] = std::make_unique<T>(std::forward<Args>(args)...);
		}
		else {
			m_resources.try_emplace(key, std::make_unique<T>(std::forward<Args>(args)...));
		}
	}
	static T& get(keyType key) {
		if constexpr (std::is_same_v<containerType, std::vector<std::unique_ptr<T>>>) {
			if (key < m_resources.size() && m_resources[key]) {
				return *(m_resources[key]);
			}
			throw std::runtime_error("Failed to find asset at index: " + keyToString(key));
		}
		else {
			auto iter = m_resources.find(key);
			if (iter != m_resources.end()) {
				return *(iter->second);
			}
			throw std::runtime_error("Failed to find asset named: " + keyToString(key));
		}
	}

	static void erase(keyType key) {
		if constexpr (std::is_same_v<containerType, std::vector<std::unique_ptr<T>>>) {
			if (key < m_resources.size()) {
				m_resources[key].reset();
			}
		}
		else {
			m_resources.erase(key);
		}
	}
	static void deleteAll() {
		m_resources.clear();
	}
};

template <typename... Types>
class assetManager : public assetPool<Types>...{
private:
	assetManager() = default;
	~assetManager() = default;
public:
	template <typename T, typename Key, typename... Args>
	static void add(Key&& key, Args&&... args) {
		assetPool<T>::add(std::forward<Key>(key), std::forward<Args>(args)...);
	}
	template <typename T, typename Key>
	static T& get(Key&& key) {
		return assetPool<T>::get(std::forward<Key>(key));
	}
	template <typename T, typename Key>
	static void erase(Key&& key) {
		return assetPool<T>::erase(std::forward<Key>(key));
	}
	template <typename T>
	static void deleteAll() {
		assetPool<T>::deleteAll();
	}
};


using mainAssetManager = assetManager<texture, shader, mesh>;