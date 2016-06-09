import java.util.Scanner;

public class ArrayStackTest {
    public static void main(String args[]) {

        // ArrayStack<String> stack = new ArrayStack<String>();
        Stack<String> stack = new Stack<String>();

        String tmpStr;
        Scanner sc = new Scanner(System.in);
        while (sc.hasNextLine()) {
            tmpStr = sc.nextLine();
            if (tmpStr.isEmpty()) {
                break;
            }
            stack.push(tmpStr);
        }

        System.out.println("iterator print");
        for (String s: stack) {
            System.out.print(" -> ");
            System.out.print(s);
        }
        System.out.println();

        System.out.println("stack print");

        while (!stack.isEmpty()) {
            System.out.print(" -> ");
            System.out.print(stack.pop());
        }

        System.out.println();
    }
}