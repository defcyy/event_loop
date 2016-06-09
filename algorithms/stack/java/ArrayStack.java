import java.util.Iterator;


public class ArrayStack<Item> implements Iterable<Item> {

    private Item[] stackArray = (Item[]) new Object[1];
    private int stackSize = 0;

    public ArrayStack() {

    }

    private void arrayResize(int newSize) {
        Item[] newArray = (Item[]) new Object[newSize];

        for (int i = 0; i < stackArray.length; i++) {
            newArray[i] = stackArray[i];
        }

        stackArray = newArray;
    }

    public boolean isEmpty () {
        return stackSize == 0;
    }

    public void push(Item item) {
        if (stackSize == stackArray.length) {
            arrayResize(2 * stackArray.length);
        }
        stackArray[stackSize++] = item;
    }

    public Item pop() {
        Item item = stackArray[--stackSize];
        stackArray[stackSize] = null;
        if (stackSize == stackArray.length * (1/4)) {
            arrayResize(stackArray.length * (1/2));
        }
        return item;
    }

    public int size() {
        return stackSize;
    }

    public Iterator<Item> iterator() {
        return new ReverseArrayIterator();
    }

    private class ReverseArrayIterator implements Iterator<Item> {
        private int i = stackSize;

        public boolean hasNext() {
            return i > 0;
        }

        public Item next() {
            return stackArray[--i];
        }

        public void remove() {

        }
    }
 
}