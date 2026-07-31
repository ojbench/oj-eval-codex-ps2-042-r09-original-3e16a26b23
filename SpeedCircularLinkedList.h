#ifndef SPEEDCIRCULARLIST_H
#define SPEEDCIRCULARLIST_H

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

constexpr int s_prime = 31;

inline int log2(int x) {
	int ans = 0;
	while (x != 1) {
		x /= 2;
		++ans;
	}
	return ans;
}

template<typename T, int b_prime>
class SpeedCircularLinkedList {
public:
	struct Node {
		std::map<std::string, T> kv_map;
		int bound = 0;
		Node* next = nullptr;
		Node** fast_search_list = nullptr;

		Node(int bound, int fast_search_list_size) : bound(bound) {
			if (fast_search_list_size > 0) {
				fast_search_list = new Node*[fast_search_list_size];
				for (int i = 0; i < fast_search_list_size; ++i) {
					fast_search_list[i] = nullptr;
				}
			}
		}

		~Node() {
			delete[] fast_search_list;
		}
	};

private:
	Node* head = nullptr;
	int fast_search_list_size = 0;
	int list_size = 0;

	static int GetHashCode(std::string str) {
		long long ans = 0;
		for (auto& ch : str) {
			ans = (ans * s_prime + ch) % b_prime;
		}
		return static_cast<int>((ans + b_prime) % b_prime);
	}

	Node* FindNodeByCode(int code) const {
		if (head == nullptr) {
			return nullptr;
		}
		if (code <= head->bound || list_size == 1) {
			return head;
		}

		Node* cur = head;
		for (int level = fast_search_list_size - 1; level >= 0; --level) {
			Node* nxt = cur->fast_search_list == nullptr ? nullptr : cur->fast_search_list[level];
			if (nxt != nullptr && nxt != head && cur->bound < nxt->bound && nxt->bound < code) {
				cur = nxt;
			}
		}
		return cur->next;
	}

	void BuildFastSearchList() {
		if (head == nullptr || fast_search_list_size == 0) {
			return;
		}

		std::vector<Node*> nodes;
		nodes.reserve(list_size);
		Node* cur = head;
		for (int i = 0; i < list_size; ++i) {
			nodes.push_back(cur);
			cur = cur->next;
		}

		for (int i = 0; i < list_size; ++i) {
			for (int k = 0; k < fast_search_list_size; ++k) {
				nodes[i]->fast_search_list[k] = nodes[(i + (1 << k)) % list_size];
			}
		}
	}

public:
	explicit SpeedCircularLinkedList(std::vector<int> node_bounds) {
		list_size = static_cast<int>(node_bounds.size());
		fast_search_list_size = list_size <= 1 ? 0 : log2(list_size);

		if (list_size == 0) {
			return;
		}

		head = new Node(node_bounds[0], fast_search_list_size);
		Node* cur = head;
		for (int i = 1; i < list_size; ++i) {
			cur->next = new Node(node_bounds[i], fast_search_list_size);
			cur = cur->next;
		}
		cur->next = head;

		BuildFastSearchList();
	}

	~SpeedCircularLinkedList() {
		if (head == nullptr) {
			return;
		}

		Node* cur = head->next;
		while (cur != head) {
			Node* nxt = cur->next;
			delete cur;
			cur = nxt;
		}
		delete head;
	}

	void put(std::string str, T value) {
		int code = GetHashCode(str);
		Node* node = FindNodeByCode(code);
		if (node != nullptr) {
			node->kv_map[std::move(str)] = std::move(value);
		}
	}

	T get(std::string str) {
		int code = GetHashCode(str);
		Node* node = FindNodeByCode(code);
		if (node == nullptr) {
			return T();
		}

		typename std::map<std::string, T>::iterator it = node->kv_map.find(str);
		if (it == node->kv_map.end()) {
			return T();
		}
		return it->second;
	}

	void print() {
		Node* cur = head;
		for (int i = 0; i < list_size; ++i) {
			std::cout << "[Node] Bound = " << cur->bound
				      << ", kv_map_size = " << cur->kv_map.size() << '\n';
			cur = cur->next;
		}
	}

	int size() const {
		return list_size;
	}
};

#endif // SPEEDCIRCULARLIST_H
