package hust.cs.javacourse.search.run;

import hust.cs.javacourse.search.index.impl.Term;
import hust.cs.javacourse.search.query.AbstractHit;
import hust.cs.javacourse.search.query.AbstractIndexSearcher;
import hust.cs.javacourse.search.query.Sort;
import hust.cs.javacourse.search.query.impl.IndexSearcher;
import hust.cs.javacourse.search.query.impl.SimpleSorter;
import hust.cs.javacourse.search.util.Config;

/**
 * 测试搜索
 */
public class TestSearchIndex {
    /**
     *  搜索程序入口
     * @param args ：命令行参数
     */
    public static void main(String[] args){
        Sort simpleSorter = new SimpleSorter();
        String indexFile = Config.INDEX_DIR + "index.dat";
        AbstractIndexSearcher searcher = new IndexSearcher();
        searcher.open(indexFile);
        AbstractHit[] hits = searcher.search(new Term("coronavirus"),
                new Term("wales"),
                simpleSorter,
                AbstractIndexSearcher.LogicalCombination.AND);

        //AbstractHit[] hits = searcher.search(new Term("coronavirus"), simpleSorter);

        if(hits == null) {
            System.out.println("Not Found");
            System.exit(1);
        }
        for(AbstractHit hit : hits){
            System.out.println(hit);
        }
    }
}
