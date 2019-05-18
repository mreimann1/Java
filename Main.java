/**
  *Tests Quick Sort
  */

public class Main {

	public static void main(String[] args) {
		testQS();
	}
	
	public static void testQS() {
		Integer a[] = new Integer [] {4,9,1,15,12,10,11};
		display(a);
		MyQuickSort.quickSort(a);
		display(a);
	}

	
	public static <T> void display(T[] a) {
		for (int i = 0; i < a.length; i++)
			System.out.print(a[i] + "    ");
		System.out.println();
	}
}
