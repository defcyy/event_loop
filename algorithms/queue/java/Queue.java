import java.util.Iterator;


public class Queue<Item> implements Iterable<Item> {

    private class Node {
        private Item item;
        private Node next;
    }

    private Node first;
    private Node last;
    private int queueSize = 0;

    public Queue() {

    }

    public boolean isEmpty() {
        return first == null;
    }

    public int size() {
        return queueSize;
    }

    public void enqueue(Item item) {
        Node node = new Node();
        node.item = item;
        node.next = null;
        if (isEmpty()) {
            first = node;
            last = first;
        }else {
            last.next = node;
            last = node;
        }

        queueSize++;
    }

    public Item dequeue() {
        Node node = first;
        first = first.next;
        queueSize--;
        return node.item;
    }

    public Iterator<Item> iterator() {
        return new QueueIterator();
    }

    private class QueueIterator implements Iterator<Item> {
        Node current = first;

        public boolean hasNext() {
            return current != null;
        }

        public Item next() {
            Item item = current.item;
            current = current.next;

            return item;
        }

        public void remove() {

        }
    }

}