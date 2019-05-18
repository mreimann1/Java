/**
 * A Class for QuickSort method
 * @author Marques Reimann
 *
 */

public class MyQuickSort {
	
	/**
	 * Calls private quickSort method to sort entire length of the array
	 */
	public static <T extends Comparable<? super T>> void quickSort(T[] a) {
		quickSort(a, 0, a.length - 1);
	}

	/**
	 * Sorts an array into ascending order. Uses quick sort with median-of-three
	 * pivot selection for arrays of at least MIN_SIZE entries, and uses insertion
	 * sort for smaller arrays.
	 */
	private static <T extends Comparable<? super T>> void quickSort(T[] a, int first, int last) {
		if ((last - first) < 3) { // use swap function to "sort" arrays of size 2
			if (a[first].compareTo(a[last]) > 0) {
				swap(a, first, last);
			}
		} else { // Create the partition: Smaller | Pivot | Larger
			int pivotIndex = partition(a, first, last);
			// Sort subarrays Smaller and Larger
			quickSort(a, first, pivotIndex - 1);
			quickSort(a, pivotIndex, last);
		} // end if
	} // end quickSort

	/**
	 * Partitions an array such that all values on left of pivot are smaller than
	 * pivot and all values on right are larger than pivot
	 * 
	 * @return index of pivot
	 */
	public static <T extends Comparable<? super T>> int partition(T[] a, int f, int l) {
		int m = l / 2; // sets middle to halfway between
		sortFirstMiddleLast(a, f, m, l);
		swap(a, m, l - 1);
		int pivotIndex = l - 1;
		T pivotValue = a[pivotIndex];
		int indexFromLeft = f + 1;
		int indexFromRight = l - 2;

		boolean done = false;
		while (!done) {
			while (a[indexFromLeft].compareTo(pivotValue) < 0)
				indexFromLeft++;
			while (a[indexFromRight].compareTo(pivotValue) > 0)
				indexFromRight--;
			if (indexFromLeft < indexFromRight) {
				swap(a, indexFromLeft, indexFromRight);
				indexFromLeft++;
				indexFromRight--;
			} else
				done = true;
		}
		swap(a, pivotIndex, indexFromLeft);
		pivotIndex = indexFromLeft;

		return pivotIndex;
	}

	/**
	 * Sorts first, middle, and last elements in array
	 */
	public static <T extends Comparable<? super T>> void sortFirstMiddleLast(T[] a, int f, int m, int l) {
		if (a[f].compareTo(a[m]) > 0)
			swap(a, f, m);
		if (a[m].compareTo(a[l]) > 0)
			swap(a, m, l);
		if (a[f].compareTo(a[m]) > 0)
			swap(a, f, m);
	}

	/**
	 * Swaps values in array at given indices
	 */
	private static <T extends Comparable<? super T>> void swap(T[] a, int x, int y) {
		T temp = a[x];
		a[x] = a[y];
		a[y] = temp;
	}
}
