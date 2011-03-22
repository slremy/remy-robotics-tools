public class runme {
  static {
	System.loadLibrary("owiswig_j");
  }
  public static void main(String argv[]) {
	usbowiarm ob = new usbowiarm(0);
	ob.test();
  }
}

