package hust.cs.javacourse.search.run;

import hust.cs.javacourse.search.index.AbstractDocumentBuilder;
import hust.cs.javacourse.search.index.AbstractIndex;
import hust.cs.javacourse.search.index.AbstractIndexBuilder;
import hust.cs.javacourse.search.index.impl.DocumentBuilder;
import hust.cs.javacourse.search.index.impl.Index;
import hust.cs.javacourse.search.index.impl.IndexBuilder;
import hust.cs.javacourse.search.util.Config;
import hust.cs.javacourse.search.util.FileUtil;

import java.io.File;

/**
 * 测试索引构建
 */
public class TestBuildIndex {
    /**
     *  索引构建程序入口
     * @param args : 命令行参数
     */
    public static void main(String[] args){
        AbstractDocumentBuilder documentBuilder = new DocumentBuilder();
        AbstractIndexBuilder indexBuilder = new IndexBuilder(documentBuilder);
        String rootDir = Config.DOC_DIR + "真实测试数据集/";
        System.out.println("Start building index.");
        AbstractIndex index = indexBuilder.buildIndex(rootDir);
        System.out.println(index);

        String indexFile = Config.INDEX_DIR + "index.dat";
        index.save(new File(indexFile));

        AbstractIndex index2 = new Index();
        index2.load(new File(indexFile));

        System.out.println("\n---------------------------------------------\n");
        System.out.println(index2);

        //写入文本文件
        String txtFile = Config.INDEX_DIR + "indexToString.txt";
        FileUtil.write(index2.toString(),txtFile);

    }
}
