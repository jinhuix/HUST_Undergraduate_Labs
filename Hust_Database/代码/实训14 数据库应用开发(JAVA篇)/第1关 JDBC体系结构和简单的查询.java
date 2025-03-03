/* 请在适当的位置补充代码，完成指定的任务 
   提示：
      try {


      } catch
    之间补充代码  
*/
import java.sql.*;

public class Client {
    public static void main(String[] args) {
        Connection connection = null;
        Statement statement = null;
        ResultSet resultSet = null;

        try {
            // 注册驱动程序
            Class.forName("com.mysql.cj.jdbc.Driver");
            // 数据库URL配置，创建数据库连接对象
            String URL = "jdbc:mysql://127.0.0.1:3306/finance?useUnicode=true&characterEncoding=UTF8&useSSL=false&serverTimezone=UTC";
            String USER = "root";
            String PASS = "123123";
            connection = DriverManager.getConnection(URL, USER, PASS);
            // 创建statement对象
            statement = connection.createStatement( );
            String SQL = "select* from client where c_mail is not null;";
            resultSet = statement.executeQuery(SQL);
            System.out.println("姓名\t邮箱\t\t\t\t电话");
            while(resultSet.next()){
                System.out.println(resultSet.getString("c_name")+'\t'+resultSet.getString("c_mail")+"\t\t"+resultSet.getString("c_phone"));
            }
         } catch (ClassNotFoundException e) {
            System.out.println("Sorry,can`t find the JDBC Driver!"); 
            e.printStackTrace();
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        } finally {
            try {
                if (resultSet != null) {
                    resultSet.close();
                }
                if (statement != null) {
                    statement.close();
                }

                if (connection != null) {
                    connection.close();
                }
            } catch (SQLException throwables) {
                throwables.printStackTrace();
            }
        }
    }
}
