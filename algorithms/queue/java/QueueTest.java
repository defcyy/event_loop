import java.util.Scanner;


public class QueueTest {
    public static void main(String args[]) {

        Queue<String> queue = new Queue<String>();
        Scanner scanner = new Scanner(System.in);

        String str;
        while(scanner.hasNextLine()) {
            str = scanner.nextLine();
            if (str.isEmpty()) {
                break;
            }

            queue.enqueue(str);
        }

        System.out.println("iterator print");
        for (String s: queue) {
            System.out.print(" -> ");
            System.out.print(s);
        }
        System.out.println();

        System.out.println("dequeue print");
        while (!queue.isEmpty()) {
            System.out.print(" -> ");
            System.out.print(queue.dequeue());
        }
        System.out.println();
    }
}