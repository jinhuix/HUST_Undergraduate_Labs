#include <string>
#include <iostream>
#include "common/exception.h"
#include "common/rid.h"
#include "storage/index/b_plus_tree.h"
#include "storage/page/header_page.h"

namespace bustub {
INDEX_TEMPLATE_ARGUMENTS
BPLUSTREE_TYPE::BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager, const KeyComparator &comparator,
                          int leaf_max_size, int internal_max_size)
    : index_name_(std::move(name)),
      root_page_id_(INVALID_PAGE_ID),
      buffer_pool_manager_(buffer_pool_manager),
      comparator_(comparator),
      leaf_max_size_(leaf_max_size),
      internal_max_size_(internal_max_size) {}


/*****************************************************************************
 * ɾ��
 *****************************************************************************/
/*
 * �������ܣ�
 *  ��B+����ɾ��key��Ӧ�ļ�¼
 * ���飺
 *  1.��B+��Ϊ��ֱ�ӷ���
 *  2.ͨ����һ���ؿ�ʵ�ֵ�FindLeafPage(key)����keyֵ����ҳ��ɾ����¼
 *  3.ɾ�������жϽ��Ԫ���Ƿ�С��min_size,������CoalesceOrRedistribute()���к�������
 *  4.ע��unpinPage�����⻺�����ڴ�й¶
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Remove(const KeyType &key) {
  std::lock_guard<std::mutex> lock(latch_);
  // ��B+��Ϊ��ֱ�ӷ���
  if (IsEmpty()) return;
  // FindLeafPage(key)����keyֵ����ҳ��ɾ����¼
  Page *page = FindLeafPage(key);
  assert(page != nullptr);
  B_PLUS_TREE_LEAF_PAGE_TYPE *leaf = reinterpret_cast<B_PLUS_TREE_LEAF_PAGE_TYPE *>(page->GetData());
  // ɾ�������жϽ��Ԫ���Ƿ�С��min_size
  // ������CoalesceOrRedistribute()���к�������
  int size = leaf->RemoveAndDeleteRecord(key, comparator_);
  if (size < leaf->GetMinSize()) CoalesceOrRedistribute(leaf);
  // ע��unpinPage�����⻺�����ڴ�й¶
  buffer_pool_manager_->UnpinPage(page->GetPageId(),true);
  return;
}

/*
 * �������ܣ�
 *  ��Ԫ�ظ���С��Min_size�Ľ����д���
 *      @param N *node ��ʾ�ڲ�����Ҷ���
 *      @return: ���н��ϲ�������true�����򷵻�false
 * ���飺
 *  1.��nodeΪ�����ʱ����Ҫ����AdjustRoot()���е���
 *  2.����Ҫͨ������FindSibling()�ҵ��ֵܽ��
 *  3.�����������Ԫ�ؿ��Ժϲ��������Coalesce()���кϲ�
 *  4.���Ԫ�ع��࣬�޷��ϲ��������Redistribute()���н��Ԫ�صĵ���
 */
INDEX_TEMPLATE_ARGUMENTS
template <typename N>
bool BPLUSTREE_TYPE::CoalesceOrRedistribute(N *node) {
  //������Ժϲ�������ҳ��ѡ��ͺϲ�
  //����޷��ϲ�����ѡ�������ҳ��ȡһ����㣬�����ֵ�FindSibling
  if (node->IsRootPage()) {
		//�ú������ܵݹ����������㣬����Ҫ�Ը����������⴦��
    return AdjustRoot(node);
  }
  N *sibling;
  // result Ϊtrue�����ʱnodeΪ��ҳ���׽��
  bool result = FindSibling(node, sibling);
  Page *parent = buffer_pool_manager_->FetchPage(node->GetParentPageId());
  B_PLUS_TREE_INTERNAL_PAGE *parentPage = reinterpret_cast<B_PLUS_TREE_INTERNAL_PAGE *>(parent->GetData());
  // �����������Ԫ�ؿ��Ժϲ��������Coalesce()���кϲ�
  if (node->GetSize() + sibling->GetSize() <= node->GetMaxSize()) {
    //�����ں�
    int removedIndex;
		//����if�жϣ���֤�ڸ�ҳɾ����keyΪ��ҳ�ķ��׽��
    if (result) {
      //��ʱ��Ҫ�Ѻ��ҳ�ϲ����׽����
      removedIndex = parentPage->ValueIndex(sibling->GetPageId());
      Coalesce(&node, &sibling, &parentPage, removedIndex);
    } else {
      //��ǰ���ϲ���ǰ�������
      removedIndex = parentPage->ValueIndex(node->GetPageId());
      Coalesce(&sibling, &node, &parentPage, removedIndex);
    }

    buffer_pool_manager_->UnpinPage(parentPage->GetPageId(), true);
    return true;
  }
  // ���Ԫ�ع��࣬�޷��ϲ��������Redistribute()���н��Ԫ�صĵ���
  int nodeIndex = parentPage->ValueIndex(node->GetPageId());
  Redistribute(sibling, node, nodeIndex);  
  buffer_pool_manager_->UnpinPage(parentPage->GetPageId(), false);
  return false;
}

/*
 * �������ܣ�
 *  ��Ԫ�ظ���С��Min_size�ĸ������и���
 *      @return: ���н�㱻ɾ��������true�����򷵻�false
 * ���飺
 *  1.����������Ҷ��㣬Ҳ�������ڲ���㣬�ֿ�����
 *  2.������������һ��Ԫ�ر�ɾ������ʱB+��Ϊ�գ�����UpdateRootPageId()���¸����page_id
 *  3.��������н�ʣ��һ��Ԫ�أ��滻����B+���ĸ����
 *  4.ע��unpinPage��deletePage
 */
INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::AdjustRoot(BPlusTreePage *old_root_node) {
    if (old_root_node->IsLeafPage()) {
    // ����������Ҷ��㣬Ҳ�������ڲ���㣬�ֿ�����
    // ������������һ��Ԫ�ر�ɾ������ʱB+��Ϊ��
    // ����UpdateRootPageId()���¸����page_id
        buffer_pool_manager_->UnpinPage(old_root_node->GetPageId(), false);
        buffer_pool_manager_->DeletePage(old_root_node->GetPageId());
        root_page_id_ = INVALID_PAGE_ID;
        UpdateRootPageId();
        return true;
    }
    if (old_root_node->GetSize() == 1) {
    // ��������н�ʣ��һ��Ԫ�أ��滻����B+���ĸ����
        B_PLUS_TREE_INTERNAL_PAGE *root = reinterpret_cast<B_PLUS_TREE_INTERNAL_PAGE *>(old_root_node);
        const page_id_t newRootId = root->RemoveAndReturnOnlyChild();
        root_page_id_ = newRootId;
        UpdateRootPageId();
        Page *page = buffer_pool_manager_->FetchPage(root_page_id_);
        assert(page != nullptr);
        B_PLUS_TREE_INTERNAL_PAGE *newRoot = reinterpret_cast<B_PLUS_TREE_INTERNAL_PAGE *>(page->GetData());
        newRoot->SetParentPageId(INVALID_PAGE_ID);
        buffer_pool_manager_->UnpinPage(root_page_id_, true);
        buffer_pool_manager_->UnpinPage(old_root_node->GetPageId(), false);
        buffer_pool_manager_->DeletePage(old_root_node->GetPageId());
    }
    return false;
}

/*
 * �������ܣ�
 *  Ѱ��node�����ֵܽ��sibling
 *      @return: ���ֵܽ��Ϊ�ҽ�㣬�򷵻�true��������Ϊfalse
 * ���飺
 *  1.�ֵܽ�������������ͬһ���ڵ��ڵĽ��
 *  2.ע��unpinPage�����⻺�����ڴ�й¶
 */
INDEX_TEMPLATE_ARGUMENTS
template <typename N>
bool BPLUSTREE_TYPE::FindSibling(N *node, N *&sibling) {
  Page *page = buffer_pool_manager_->FetchPage(node->GetParentPageId());
  B_PLUS_TREE_INTERNAL_PAGE *parent = reinterpret_cast<B_PLUS_TREE_INTERNAL_PAGE *>(page->GetData());
  int index = parent->ValueIndex(node->GetPageId());
  int siblingIndex = index - 1;
  //����Ǹ�ҳ�Ľ���Ǹ�ҳ���׽�㣬��Ӻ��ҳ��һ����������ǣ����ǰ��ҳ��һ�����������Ա�֤Ӱ�췶Χ������ͬһ����ҳ�ڣ�������
  if (index == 0) {
    siblingIndex = index + 1;
  }
  page = buffer_pool_manager_->FetchPage(parent->ValueAt(siblingIndex));
  sibling = reinterpret_cast<N *>(page->GetData());
  buffer_pool_manager_->UnpinPage(parent->GetPageId(), false);
  return index == 0;
}
/*
 * �������ܣ�
 *  ��node�е�Ԫ��ȫ���ϲ���neighbor_node��
 *      @param   neighbor_node      �Ҵ���
 *      @param   node               ���ϲ����
 *      @param   parent             ���ߵĸ����
 *      @param   index              ���ϲ�����ڸ�����е�����
 *      @return: ���н�㱻ɾ��������true�����򷵻�false
 * ���飺
 *  1.node�ϲ�����Ը���������ص���
 *  2.�����Ԫ�ر�ɾ������С��Min_size�����CoalesceOrRedistribute����
 *  3.ע��unpinPage��deletePage
 */
INDEX_TEMPLATE_ARGUMENTS
template <typename N>
bool BPLUSTREE_TYPE::Coalesce(N **neighbor_node, N **node,
                              BPlusTreeInternalPage<KeyType, page_id_t, KeyComparator> **parent, int index) {
  assert((*node)->GetSize() + (*neighbor_node)->GetSize() <= (*node)->GetMaxSize());
  (*node)->MoveAllTo(*neighbor_node,buffer_pool_manager_);
  page_id_t pId = (*node)->GetPageId();
  buffer_pool_manager_->UnpinPage(pId, true);
  buffer_pool_manager_->DeletePage(pId);
  buffer_pool_manager_->UnpinPage((*neighbor_node)->GetPageId(), true);
  (*parent)->Remove(index);
	//���ڲ�ҳ��size��Сʱ���߽�ʣ��һ����Ч���ʱ���ݹ鴥��CoalesceOrRedistribute�������Ե�ǰ�����д���
  if ((*parent)->GetSize() < (*parent)->GetMinSize() || (*parent)->GetSize() == 2) {
    return CoalesceOrRedistribute(*parent);
  }
  return false;
}

/*
 * �������ܣ�
 *  ��node�е�Ԫ���ƶ���neighbor_node��
 * ���飺
 *  1.��indexΪ0����neighbor_node��Ԫ���ƶ���nodeβ��
 *  ����neighbor_node��βԪ���ƶ���node�ײ�
 *  2.ע��unpinPage�����⻺�����ڴ�й¶
 */
INDEX_TEMPLATE_ARGUMENTS
template <typename N>
void BPLUSTREE_TYPE::Redistribute(N *neighbor_node, N *node, int index) {
  if (index == 0) {
    neighbor_node->MoveFirstToEndOf(node, buffer_pool_manager_);
  } else {
    neighbor_node->MoveLastToFrontOf(node, buffer_pool_manager_);
  }
  buffer_pool_manager_->UnpinPage(node->GetPageId(), true);
  buffer_pool_manager_->UnpinPage(neighbor_node->GetPageId(), true);
}

/*
 * �������ܣ�
 *  ��B+������㷢���仯ʱ�����øú�����header_page�жԸ����ID���и���
 *      @param insert_record ȱʡʱinsert_recordΪ0
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::UpdateRootPageId(int insert_record) {
    HeaderPage *header_page = static_cast<HeaderPage *>(buffer_pool_manager_->FetchPage(HEADER_PAGE_ID));
    if (insert_record != 0) {
        // create a new record<index_name + root_page_id> in header_page
        header_page->InsertRecord(index_name_, root_page_id_);
    } else {
        // update root_page_id in header_page
        header_page->UpdateRecord(index_name_, root_page_id_);
    }
    buffer_pool_manager_->UnpinPage(HEADER_PAGE_ID, true);
}

template class BPlusTree<GenericKey<4>, RID, GenericComparator<4>>;
template class BPlusTree<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTree<GenericKey<16>, RID, GenericComparator<16>>;
template class BPlusTree<GenericKey<32>, RID, GenericComparator<32>>;
template class BPlusTree<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub