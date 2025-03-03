package hust.cs.javacourse.search.parse.impl;

import hust.cs.javacourse.search.index.AbstractTermTuple;
import hust.cs.javacourse.search.parse.AbstractTermTupleFilter;
import hust.cs.javacourse.search.parse.AbstractTermTupleStream;
import hust.cs.javacourse.search.util.Config;

/**
 * PatternTermTupleFilter是抽象类AbstractTermTupleFilter的子类.
 * 选择英文字母组成的Term对应的三元组
 */
public class PatternTermTupleFilter extends AbstractTermTupleFilter {
    /**
     * 构造函数
     *
     * @param input ：Filter的输入，类型为AbstractTermTupleStream
     */
    public PatternTermTupleFilter(AbstractTermTupleStream input) {
        super(input);
    }

    /**
     * 获得下一个三元组
     *
     * @return: 下一个三元组；如果到了流的末尾，返回null
     */
    @Override
    public AbstractTermTuple next() {
        AbstractTermTuple temp = input.next();
        while (temp!=null){
            if(temp.term.getContent().matches(Config.TERM_FILTER_PATTERN)){
                return temp;
            }
            else{
                temp = input.next();
            }
        }
        return null;
    }
}
