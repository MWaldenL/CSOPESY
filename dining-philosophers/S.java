public class S {
  static int[] state = {0,0,0,0,0};

  public static void printState() {
    System.out.println("\nCurrent State: ");
    for (int s: state) {
      System.out.print(s + " ");
    }
    System.out.println("\n");
  }
}
