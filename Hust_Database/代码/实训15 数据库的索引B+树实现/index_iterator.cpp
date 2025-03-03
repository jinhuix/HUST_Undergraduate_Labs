#include <cassert>
#include "storage/index/index_iterator.h"

namespace bustub {

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::IndexIterator(B_PLUS_TREE_LEAF_PAGE_TYPE *leaf, int index, BufferPoolManager *bufferPoolManager)
    : index_(index), leaf_(leaf), bufferPoolManager_(bufferPoolManager) {}
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::IndexIterator() = default;
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::~IndexIterator(){
	if (leaf_ != nullptr) {
    bufferPoolManager_->UnpinPage(leaf_->GetPageId(),false);
  }
}
/*
 * �������ܣ�
 *  �жϵ������Ƿ��Ѿ���Ҷ���ȫ������
 */
INDEX_TEMPLATE_ARGUMENTS
bool INDEXITERATOR_TYPE::IsEnd() { 
	//�п��ܴ���ĩβ��Ҳ�п����ǿ�
	return ((leaf_ == nullptr || index_ >= leaf_->GetSize()) ) ;
}
/*
 * �������ܣ�
 *  ���ص������ڵ�ǰҳ����ָ��ľ����ֵ��
 */
INDEX_TEMPLATE_ARGUMENTS
const MappingType &INDEXITERATOR_TYPE::operator*() { 
	return leaf_->GetItem(index_);
}

/*
 * �������ܣ�
 *  �������ƶ�����һ����¼(ʵ�ֵ�����˳�����B+����¼�Ĺ���)�����ص�ǰ������
 * ���飺
 *  1.����NextPageId����ʵ�ֿ������
 *  2.����¼������(NextPageId == INVALID_PAGE_ID),��IsEnd()����true
 *  3.��Ҷ�ӽ�㲻��ʹ�ú���Ҫ��ʱunpin�ͷţ����⻺�����ڴ�й¶
 */
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE &INDEXITERATOR_TYPE::operator++() {
	//�õ�������Ҷ�ӽڵ�ĵ�������B+����Ҷ�ӽڵ�֮���໥���ӣ���ʵ��˳�����������
	//�ʴ˵�����Ӧ�ÿ����ڲ�ͬ�Ľ����ƶ�
  if ((index_ + 1) >= leaf_->GetSize()) {
    // ����NextPageId����ʵ�ֿ������
    page_id_t next = leaf_->GetNextPageId();
    if (next == INVALID_PAGE_ID) {
      // Ҷ�ӽ�㲻��ʹ�ú���Ҫ��ʱunpin�ͷ�
      bufferPoolManager_->UnpinPage(leaf_->GetPageId(), false);
      leaf_ = nullptr;
      index_ = 0;
      return *this;
    } else {
      bufferPoolManager_->UnpinPage(leaf_->GetPageId(), false);
      Page *page = bufferPoolManager_->FetchPage(next);
      leaf_ = reinterpret_cast<B_PLUS_TREE_LEAF_PAGE_TYPE *>(page);
      index_ = 0;
      return *this;
    }
  } else {
    index_++;
    return *this;
  }
}

template class IndexIterator<GenericKey<4>, RID, GenericComparator<4>>;

template class IndexIterator<GenericKey<8>, RID, GenericComparator<8>>;

template class IndexIterator<GenericKey<16>, RID, GenericComparator<16>>;

template class IndexIterator<GenericKey<32>, RID, GenericComparator<32>>;

template class IndexIterator<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub