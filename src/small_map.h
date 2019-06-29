#ifndef SMALL_MAP_H
#define SMALL_MAP_H

#include "global.h"

namespace wiz {

	template <class Key, class Data>
	class SmallMap
	{
	private:
		mutable std::vector<wiz::Pair<Key, Data>> _map;
	public:
		using iterator = typename std::vector<wiz::Pair<Key, Data>>::iterator;
		using const_iterator = typename std::vector<wiz::Pair<Key, Data>>::const_iterator;
	public:
		void insert(std::pair<Key, Data>&& item) {
			for (int i = 0; i < _map.size(); ++i) {
				if (_map[i].first == item.first) {
					_map[i] = wiz::Pair(std::move(item.first), std::move(item.second));
					return;
				}
			}
			_map.push_back(wiz::Pair(std::move(item.first), std::move(item.second)));
		}
		void insert(const std::pair<Key, Data>& item) {
			for (int i = 0; i < _map.size(); ++i) {
				if (_map[i].first == item.first) {
					_map[i] = wiz::Pair(item.first, item.second);
					return;
				}
			}
			_map.push_back(wiz::Pair(item.first, item.second));
		}

		auto find(Key&& key) {
			return std::find(_map.begin(), _map.end(), wiz::Pair(std::move(key), Data()));
		}
		auto find(Key&& key) const {
			return std::find(_map.begin(), _map.end(), wiz::Pair(std::move(key), Data()));
		}
		auto find(const Key& key) {
			return std::find(_map.begin(), _map.end(), wiz::Pair(key, Data()));
		}
		auto find(const Key& key) const {
			return std::find(_map.begin(), _map.end(), wiz::Pair(key, Data()));
		}

		Data& operator[] (Key&& key) {
			for (int i = 0; i < _map.size(); ++i) {
				if (_map[i].first == key) {
					return _map[i].second;
				}
			}

			_map.push_back(wiz::Pair(std::move(key), Data()));
			return _map.back().second;
		}
		const Data& operator[] (Key&& key) const {
			for (int i = 0; i < _map.size(); ++i) {
				if (_map[i].first == key) {
					return _map[i].second;
				}
			}

			_map.push_back(wiz::Pair(std::move(key), Data()));
			return _map.back().second;
		}

		Data& operator[] (const Key& key) {
			for (int i = 0; i < _map.size(); ++i) {
				if (_map[i].first == key) {
					return _map[i].second;
				}
			}

			_map.push_back(wiz::Pair(key, Data()));
			return _map.back().second;
		}
		const Data& operator[] (const Key& key) const {
			for (int i = 0; i < _map.size(); ++i) {
				if (_map[i].first == key) {
					return _map[i].second;
				}
			}

			_map.push_back(wiz::Pair(key, Data()));
			return _map.back().second;
		}

		void clear() {
			_map.clear();
		}

		auto size() const {
			return _map.size();
		}

		auto begin() {
			return _map.begin();
		}
		auto begin() const {
			return _map.begin();
		}
		auto end() {
			return _map.end();
		}
		auto end() const {
			return _map.end();
		}

		auto at(Key&& key) {
			return (*this)[std::move(key)];
		}
		auto at(Key&& key) const {
			return (*this)[std::move(key)];
		}
		auto at(const Key& key) {
			return (*this)[key];
		}
		auto at(const Key& key) const {
			return (*this)[key];
		}
	};
}

#endif
