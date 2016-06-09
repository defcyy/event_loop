import java.util.Iterator;


public class Stack<Item> implements Iterable<Item> {
    private class Node {
        private Item item;
        private Node next;
    }

    private int stackSize = 0;
    private Node first;

    public boolean isEmpty() {
        return stackSize == 0;
    }

    public int size() {
        return stackSize;
    }

    public void push(Item item) {
        Node node = new Node();
        node.item = item;
        node.next = first;
        first = node;
        stackSize++;
    }

    public Item pop() {
        Item item = first.item;
        first = first.next;
        stackSize--;
        return item;
    }

    public Iterator<Item> iterator() {
        return new StackIterator();
    }


    private class StackIterator implements Iterator<Item> {
        private Node node = first;

        public boolean hasNext() {
            return  node != null;
        }

        public Item next() {
            Item item = node.item;
            node = node.next;
            return item;
        }

        public void remove() {

        }
    }
}