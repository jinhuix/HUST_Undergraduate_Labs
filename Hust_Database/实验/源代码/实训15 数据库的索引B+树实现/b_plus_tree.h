/**
 * ��ͷ�ļ��޸���Ч�������Ķ��ο�����Ҫ�޸�
 */
#pragma once

#include <queue>
#include <string>
#include <vector>

#include "concurrency/transaction.h"
#include "storage/index/index_iterator.h"
#include "storage/page/b_plus_tree_internal_page.h"
#include "storage/page/b_plus_tree_leaf_page.h"

namespace bustub {

#define BPLUSTREE_TYPE BPlusTree<KeyType, ValueType, KeyComparator>

/**
 * B+��������
 * (1) key���ظ�������Ψһ
 * (2) ֧�ֲ����ɾ������(�ùؿ���ʵ�ֲ���)
 * (4) ��ʵ��B+��index_iterator�࣬��B+������ʹ��
 */
    INDEX_TEMPLATE_ARGUMENTS
    class BPlusTree {

    public:
        explicit BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager, const KeyComparator &comparator,
                           int leaf_max_size = LEAF_PAGE_SIZE, int internal_max_size = INTERNAL_PAGE_SIZE);

        bool IsEmpty() const;

        bool Insert(const KeyType &key, const ValueType &value);

        bool GetValue(const KeyType &key, std::vector<ValueType> *result);

        //index_iterator
        INDEXITERATOR_TYPE Begin();
        INDEXITERATOR_TYPE Begin(const KeyType &key);
        INDEXITERATOR_TYPE End();

        Page *FindLeafPage(const KeyType &key, bool leftMost = false);

    private:
        void StartNewTree(const KeyType &key, const ValueType &value);

        bool InsertIntoLeaf(const KeyType &key, const ValueType &value);

        void InsertIntoParent(BPlusTreePage *old_node, const KeyType &key, BPlusTreePage *new_node);

        template <typename N>
        N *Split(N *node);

        void UpdateRootPageId(int insert_record = 0);


        // member variable
        //��������
        std::string index_name_;
        //���ڵ��PageID
        page_id_t root_page_id_;
        BufferPoolManager *buffer_pool_manager_;
        KeyComparator comparator_;
        //Ҷ���������Ŀ
        int leaf_max_size_;
        //�ڲ����������Ŀ
        int internal_max_size_;
        std::mutex latch_;
    };

}
