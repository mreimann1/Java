/*
 * Calls various testing functions
 * @author Marques Reimann
 */

public class Main {
	public static void main(String[] args) {
		testBST();
	}
	
	/*
	 * Tests BinarySearchTree class
	 */
	public static void testBST(){
		BinarySearchTree<Integer> bst = new BinarySearchTree<>();
		bst.insert(5);
		bst.insert(7);
		bst.insert(2);
		bst.insert(9);
		bst.insert(3);
		bst.insert(1);
		bst.displayIn();
		bst.displayPre();
		bst.displayPost();
		System.out.println("The count is: " + bst.countNodes() 
							+ "\t\tThe number of levels is: " + bst.countLevels());
		bst.testNTR();
		bst.testFN(2);
	}
}
