#include <iostream>
#include <sstream>

#include "common/exception.h"
#include "storage/page/b_plus_tree_internal_page.h"

namespace bustub {
/*
 * �������ܣ�
 *  ��ʼ��thisҳ������page type,size,page id,parent id and max page size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::Init(page_id_t page_id, page_id_t parent_id, int max_size) {
  // ��ʼ��page type
  SetPageType(IndexPageType::INTERNAL_PAGE);
  // ��ʼ��size
  SetSize(0);
  // ��ʼ��page id
  SetPageId(page_id);
  // ��ʼ��parent id
  SetParentPageId(parent_id);
  // ��ʼ��max page size
  SetMaxSize(max_size);
}

/*
 * �������ܣ�
 *  ����index����keyֵ
 * ���飺
 *  1.������Ԫ��Ϊpair��firstΪkey��secondΪRID
 */
INDEX_TEMPLATE_ARGUMENTS
KeyType B_PLUS_TREE_INTERNAL_PAGE_TYPE::KeyAt(int index) const {
  // ������Ԫ��Ϊpair��firstΪkey��secondΪRID
  return array_[index].first;
}

INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::SetKeyAt(int index, const KeyType &key) { 
	array_[index].first = key;
  return;
}

/*
 * �������ܣ�
 *  Ѱ�ҵ�ǰ������еļ�ֵ����ֵΪvalue��Ԫ�ص�����
 * ���飺
 *  1.���δ�ҵ�������-1
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_INTERNAL_PAGE_TYPE::ValueIndex(const ValueType &value) const {
  for (int i = 0; i < GetSize(); i++) {
    if (value != ValueAt(i)) continue;
    // �ҵ�������Ԫ�ص�����
    return i;
  }
  // δ�ҵ�������-1
  return -1;
}

INDEX_TEMPLATE_ARGUMENTS
ValueType B_PLUS_TREE_INTERNAL_PAGE_TYPE::ValueAt(int index) const {
  return array_[index].second;
}


/*
 * �������ܣ�
 *  ��keyֵ�������е������У��ҵ��ض�keyֵ��Ӧ��value
 * ���飺
 *  1.ѡ����ʲ��ҷ�
 *  2.���keyֵ�����ڣ�������ӽ��Ľ�Сkey��Ӧ��value
 *  3.ʹ��comparator����keyֵ�ıȽ�
 */
INDEX_TEMPLATE_ARGUMENTS
ValueType B_PLUS_TREE_INTERNAL_PAGE_TYPE::Lookup(const KeyType &key, const KeyComparator &comparator) const {
  assert(GetSize() > 1);
	int low = 0;
  int high = GetSize() - 1;
  int mid;
  // ʹ�ö��ֲ���
  while(low <= high) {
    mid = (low + high) / 2;
    if (comparator(array_[mid].first, key) < 0)
      low = mid + 1;
    else if (comparator(array_[mid].first, key) > 0)
      high = mid - 1;
    else
      return array_[mid].second;
  }
  // ���keyֵ�����ڣ�������ӽ��Ľ�Сkey��Ӧ��value
  return array_[high].second;
}

/*****************************************************************************
 * ����
 *****************************************************************************/
/*
 * �������ܣ�
 *  ���������Ԫ�ع���ʱ�����ڷ��Ѻ��Ϊ�ڲ���㣬��Ҫ���µĸ�����ʼ��
 * ���飺
 *  1.�ú�������InsertIntoParent()(b_plus_tree.cpp)�е���
 *  2.��ʼ����������Ԫ��Ϊ[<invalid, old_value>, <new_key, new_value>]
 *  3.����size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::PopulateNewRoot(const ValueType &old_value, const KeyType &new_key,
                                                     const ValueType &new_value) {
  // �²�ú���Ϊ���ڵײ�䶯�����µĸ�Ԫ�أ���ʱ�Ըø�Ԫ�ؽ�������
  // ��ʼ����������Ԫ��Ϊ[<invalid, old_value>, <new_key, new_value>]
  array_[0].second = old_value;
  array_[1].first = new_key;
  array_[1].second = new_value;
  SetSize(2);
  return;
}
/*
 * �������ܣ�
 *  ��old_valueԪ��֮������µļ�ֵ��
 * ���飺
 *  1.��ֵ�����������Լ����ӹ�ϵ�˴����账�������������ⲿ����
 *  2.���old_value�����ڣ�����-1
 *  3.return size
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_INTERNAL_PAGE_TYPE::InsertNodeAfter(const ValueType &old_value, const KeyType &new_key,
                                                    const ValueType &new_value) {
  //���ײ�Ԫ�ط��Ѻ���Ҫ�ڸ��ڵ�ĵ�ǰ�ײ�Ԫ��������old_value��������½ڵ�ָ����Ѻ���µײ�ڵ�
  int old_index = ValueIndex(old_value);
  // old_valueԪ�ش��ڣ���֮������µļ�ֵ��
  if (old_index != -1) {
    int i;
    // ����old_index�����Ԫ�������һλ
		for (i = GetSize(); i - 1 != old_index; i--) {
      array_[i] = array_[i - 1];
    }
    // �����µļ�ֵ��
    array_[i].first = new_key; 
    array_[i].second  = new_value; 
		IncreaseSize(1);
    return GetSize();
  }
  // old_value�����ڣ�����-1
	return -1;
}

/*****************************************************************************
 * ����
 *****************************************************************************/
/*
 *�������ܣ�
 * �ƶ�this�����һ�루����ȡ������Ԫ�ص�һ�������ɵ�recipient�����
 *���飺
 *  1.����ʵ��CopyNFrom����
 *  2.���ڲ�����CopyNFrom����ʵ�ָù���
 *  3.һ�����ڵ�ǰ�����Ԫ�����ʱ���з��ѣ�������Ԫ�������½���С������ں����ⲿ���и����Ԫ�صĸ��£�
 *  eg:
 *    this == r1,  recipient == r2
 *    r1->[<invalid, p0>, <1, p1>, <2, p2>, <3, p3>, <4, p4>] ----MoveHalfTo--> r2[]
 *    result: r1->[<invalid, p0>, <1, p1>],r2[<2, p2>, <3, p3>, <4, p4>]
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::MoveHalfTo(BPlusTreeInternalPage *recipient,
                                                BufferPoolManager *buffer_pool_manager) {
  int size = GetSize();
  int remain_size = size / 2;
  // ��item����ʼ����ȡsize����ֵ�Բ��뵽����β��
  recipient->CopyNFrom(array_ + remain_size, size - remain_size, buffer_pool_manager);
  // ����size
  SetSize(remain_size);
  return;
}

/*
 * �������ܣ�
 *  ��item����ʼ����ȡsize����ֵ�Բ��뵽����β��
 * ���飺
 *  1.���ڸý�������ڲ���㣬�����Ԫ��Ǩ�ƺ���Ҫ����Ԫ�ص�ParentPageId���и��£������ӽ�����fetch�����º���unpin
 *  2.����size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::CopyNFrom(MappingType *items, int size, BufferPoolManager *buffer_pool_manager) {
	//�ڲ��ڵ������ָ������Ԫ�ؽ�㣬��Ǩ�ƺ������ϵ�Բ���
	//���������ڲ��ڵ�һ����Ǩ�Ƶ����µĽ�㣬���ⲿ�ֵ���Ԫ�ؽ��ĸ�Ԫ����Ҫ����
	//����Ҫ��page�Ӵ�����ȡ������bufferpool
  int size_ = GetSize();
  for (int i = size_; i < size_ + size; i++) {
    array_[i] = *items++;
    Page *page = buffer_pool_manager->FetchPage(array_[i].second);
    BPlusTreePage *bp_tree_page = reinterpret_cast<BPlusTreePage *>(page->GetData());
    // ����Ԫ�ص�ParentPageId���и���
    bp_tree_page->SetParentPageId(GetPageId());
    buffer_pool_manager->UnpinPage(array_[i].second, true);
    IncreaseSize(1);
  }
}

/*****************************************************************************
 * �Ƴ�
 *****************************************************************************/
/*
 * �������ܣ�
 *  1.�Ƴ�this����е�indexԪ��
 * ���飺
 *  1.ע��Ԫ�ص��ƶ�
 *  2.����size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::Remove(int index) {
  for (int i = index; i < GetSize() - 1; ++i) {
    // �����Ԫ����ǰ��λ
    array_[i] = array_[i + 1];
  }
  // ����size
  IncreaseSize(-1);
}

/*
 * �������ܣ�
 *  ���ظ���㺬�е�ΨһԪ��ValueAt(0)
 * ���飺
 *  1.�ú���ֻ��AdjustRoot()(in b_plus_tree.cpp)�е���
 */
INDEX_TEMPLATE_ARGUMENTS
ValueType B_PLUS_TREE_INTERNAL_PAGE_TYPE::RemoveAndReturnOnlyChild() { 
	IncreaseSize(-1);
  return ValueAt(0);
}
/*****************************************************************************
 * �ϲ�
 *****************************************************************************/
/*
 * �������ܣ�
 *  �ϲ�this����Ԫ����recipient����У�����this����е�ȫ��Ԫ������recipient����β��
 * ���飺
 *  1.this���ı仯Ӱ�������ӽ�㣨�ӽ��ĸ����������Ҫ���и��£�
 *  2.����recipient����size��С
 *  3.this���ı仯Ӱ�����丸��㣨�������ָ��ý��ļ�ֵ����Ҫ�������,�˲����ں�������У�����Ҫ�ڴ˴�ʵ��
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::MoveAllTo(BPlusTreeInternalPage *recipient,
                                               BufferPoolManager *buffer_pool_manager) {

  int start = recipient->GetSize();
  page_id_t recipPageId = recipient->GetPageId();


  // the separation key from parent
	//the separation key��valueָ��ý�㣬���ý����Ԫ�ص�ֵ�����ڸ�value
	//�ý�����Ԫ����keyֵ���޷�ֱ�Ӻϲ���recipient����Ԫ����ָ���ֵ��the separation key���һ��Ԫ��С������Ԫ�ص�keyʵ���Ͼ���the separation key��
  Page *page = buffer_pool_manager->FetchPage(GetParentPageId());
  assert(page != nullptr);
  BPlusTreeInternalPage *parent = reinterpret_cast<BPlusTreeInternalPage *>(page->GetData());
  // �������ָ��ý��ļ�ֵ����Ҫ�����
  int index = parent->ValueIndex(GetPageId());
  SetKeyAt(0,parent->KeyAt(index));
  buffer_pool_manager->UnpinPage(parent->GetPageId(), false);
  for (int i = 0; i < GetSize(); ++i) {
    recipient->array_[start + i].first = array_[i].first;
    recipient->array_[start + i].second = array_[i].second;
    auto childRawPage = buffer_pool_manager->FetchPage(array_[i].second);
    BPlusTreePage *childTreePage = reinterpret_cast<BPlusTreePage *>(childRawPage->GetData());
    // �ӽ��ĸ����������Ҫ���и���
    childTreePage->SetParentPageId(recipPageId);
    buffer_pool_manager->UnpinPage(array_[i].second, true);
  }
  // ����recipient����size��С
  recipient->SetSize(start + GetSize());
  assert(recipient->GetSize() <= GetMaxSize());
  SetSize(0);
}

/*****************************************************************************
 * �ط���
 *****************************************************************************/
/*
 * �������ܣ�
 *  �ƶ�this������Ԫ����recipient����ĩβ
 * ���飺
 *  1.recipient����CopyLastFrom����ʵ�ֽڵ����
 *  2.this������size
 *  3.����this�����Ԫ�ر仯����˸������this����������Ҫ����
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::MoveFirstToEndOf(BPlusTreeInternalPage *recipient,
                                                      BufferPoolManager *buffer_pool_manager) {
  Page *page = buffer_pool_manager->FetchPage(GetParentPageId());
  assert(page != nullptr);
  BPlusTreeInternalPage *parent = reinterpret_cast<BPlusTreeInternalPage *>(page->GetData());
  int index = parent->ValueIndex(GetPageId());
  SetKeyAt(0, parent->KeyAt(index));
  MappingType pair{KeyAt(0), ValueAt(0)};
  // this������size
  IncreaseSize(-1);
	// recipient����CopyLastFrom����ʵ�ֽڵ����
  // ����ռ�������ǰ�ƶ�һ����λ
  memmove(array_, array_ + 1, static_cast<size_t>(GetSize() * sizeof(MappingType)));
  recipient->CopyLastFrom(pair, buffer_pool_manager);
	//���ƶ�ͷԪ�غ������½����Ҫ����
	//�ý������ͷԪ�ط����˱仯������ЧԪ�ظı䣩����ʱ�����ָ��ý���Ԫ��keyֵ��Ҫ������Ϊ�µ���ЧԪ�ص�keyֵ��������Ԫ��
  parent->SetKeyAt(parent->ValueIndex(GetPageId()), array_[0].first);
  buffer_pool_manager->UnpinPage(GetParentPageId(), true);
	return;
}

/*
 * �������ܣ�
 *  ��pairԪ����ӵ�this����β��
 * ���飺
 *  1.����pairԪ�صĸ������Ϣ
 *  2.����size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::CopyLastFrom(const MappingType &pair, BufferPoolManager *buffer_pool_manager) {
  array_[GetSize()] = pair;
  // ����size
  IncreaseSize(1);
  // ����pairԪ�صĸ������Ϣ
  page_id_t childPageId = pair.second;
  Page *page = buffer_pool_manager->FetchPage(childPageId);
  assert(page != nullptr);
  BPlusTreePage *child = reinterpret_cast<BPlusTreePage *>(page->GetData());
  child->SetParentPageId(GetPageId());
  buffer_pool_manager->UnpinPage(child->GetPageId(), true);
  return;
}
/*
 * �������ܣ�
 *  �ƶ�this����β��Ԫ����recipient����ͷ��
 * ���飺
 *  1.recipient����CopyFirstFrom����ʵ�ֽڵ����
 *  2.this������size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::MoveLastToFrontOf(BPlusTreeInternalPage *recipient,
                                                       BufferPoolManager *buffer_pool_manager) {
  MappingType pair{KeyAt(GetSize() - 1), ValueAt(GetSize() - 1)};
  IncreaseSize(-1);
  recipient->CopyFirstFrom(pair, buffer_pool_manager);
  return;
}
/*
 * �������ܣ�
 *  �ƶ�pairԪ����recipient�����ײ�
 * ���飺
 *  1.������Ԫ�ر仯����Ҫ�Ը�����е��������и���
 *  2.����size
 *  3.����pairԪ�صĸ������Ϣ
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::CopyFirstFrom(const MappingType &pair, BufferPoolManager *buffer_pool_manager) {
  Page *page = buffer_pool_manager->FetchPage(GetParentPageId());
  assert(page != nullptr);
  // �Ը�����е��������и���
  BPlusTreeInternalPage *parent = reinterpret_cast<BPlusTreeInternalPage *>(page->GetData());
  int index = parent->ValueIndex(GetPageId());
  SetKeyAt(0, parent->KeyAt(index));

	memmove(array_ + 1, array_, GetSize() * sizeof(MappingType));
  array_[0].first = pair.first;
  array_[0].second = pair.second;
  // ����size
  IncreaseSize(1);
  page_id_t childPageId = pair.second;
  page = buffer_pool_manager->FetchPage(childPageId);
  assert(page != nullptr);
  BPlusTreePage *child = reinterpret_cast<BPlusTreePage *>(page->GetData());
  // ����pairԪ�صĸ������Ϣ
  child->SetParentPageId(GetPageId());
  buffer_pool_manager->UnpinPage(child->GetPageId(), true);

  parent->SetKeyAt(parent->ValueIndex(GetPageId()), array_[0].first);
  buffer_pool_manager->UnpinPage(GetParentPageId(), true);
  return;
}

template class BPlusTreeInternalPage<GenericKey<4>, page_id_t, GenericComparator<4>>;
template class BPlusTreeInternalPage<GenericKey<8>, page_id_t, GenericComparator<8>>;
template class BPlusTreeInternalPage<GenericKey<16>, page_id_t, GenericComparator<16>>;
template class BPlusTreeInternalPage<GenericKey<32>, page_id_t, GenericComparator<32>>;
template class BPlusTreeInternalPage<GenericKey<64>, page_id_t, GenericComparator<64>>;
}