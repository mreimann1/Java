public class Main {
	public static <T> void display(T[] arr, int size) {
		for (int i = 0; i < size; i++)
			System.out.print(arr[i] + "    ");
		System.out.println();
	}

	public static void main(String[] args) {
		Integer list[] = new Integer[5];
		list[0] = 4;
		list[1] = 9;
		list[2] = 1;
		list[3] = 15;
		list[4] = 12;
		System.out.println("Here is the array before sorting: ");
		display(list, 5);
		MergeSort.mergeSort(list, 5);
		System.out.println("Here is the array after sorting: ");
		display(list, 5);
	}
}
