import java.util.Scanner;

public class ArrayStackTest {
    public static void main(String args[]) {

        ArrayStack<String> stack = new ArrayStack<String>();

       String tmpStr;
        Scanner sc = new Scanner(System.in);
        while (sc.hasNextLine()) {
            tmpStr = sc.nextLine();
            if (tmpStr.isEmpty()) {
                break;
            }
            stack.push(tmpStr);
        }

        for (String s: stack) {
            System.out.print(" -> ");
            System.out.print(s);
        }

        System.out.println();
    }
}