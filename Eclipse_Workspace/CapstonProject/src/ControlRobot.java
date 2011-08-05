
public class ControlRobot {
	//String filePath = "xml/NRLAB02.xml";
	final int VELOCITY_MODE = 1;
	final int  POSITION_MODE = 2;		// NOT Available..... (Future update...)
	final int  POSITION_VELOCITY_MODE = 3;
	final int  TORQUE_MODE = 4;  		// NOT Available..... (Future update...)
	//final float  PI = 3.1415F;
	final float  GEAR_RATIO = 30.0F;	//30:1
	final float  WHL_RADIUS = 114.3F; 	//114.3mm
	///////////////////���� ���� //////////
	final int  INIT	= 1;                // �ʱ�ȭ ����
	final int  PREOP = 2;               // ������ �Է¸� ������ ����
	final int  SAFEOP = 4;              // ������ �Է¸� ������ ����
	final int  OP = 8;                  // ���� ���� ���� : ����� ����
	/////////////////////////////////////
	final int  MPS = 1;
	final int  MMPS = 2;
	
	public float velocity;
	public boolean clockwise;
	public int[] psdData;  
	
	public ControlRobot(){
		velocity = 0.0F;
		clockwise = true;
		psdData = new int[9];
	}
	
	static {
    	System.loadLibrary("NRLAB02v01");
        System.loadLibrary("ControlRobotLib");
    }
	
	//////////////////  Funtion  //////////////////////////
    public boolean enableNetworkUsePathString(String filePath){
    	byte[] path = filePath.getBytes();
    	return enableNetwork(path, path.length) ;
	}
    
    public boolean enableNetworkUseMacAddrString(String macAddress, String filePath){
    	byte[] address = macAddress.getBytes();
    	byte[] path = filePath.getBytes();
    	return enableNetwork(address, path, address.length, path.length) ;
    }
    
    public void restartNetworkUsePathString(String filePath){
    	byte[] path = filePath.getBytes();
    	restartNetwork(path, path.length) ;
	}
    
    public void getPsdDataClockwiseInnerVariable(){
    	int[] temporary;
    	temporary = new int[10];
    	
    	temporary = getPsdDataClockwise();
    	
    	for(int iteration = 0; iteration < psdData.length; iteration++){
    		psdData[iteration] = temporary[iteration];
    	}
    	
    	clockwise = (temporary[temporary.length-1] == 1);
    }
    
    public void getPsdDataAnticlockwiseInnerVariable(){
    	int[] temporary;
    	temporary = new int[10];
    	
    	temporary = getPsdDataAnticlockwise();
    	
    	for(int iteration = 0; iteration < psdData.length; iteration++){
    		psdData[iteration] = temporary[iteration];
    	}
    	
    	clockwise = (temporary[temporary.length-1] == 1);    	
    }
    
    /////////////////// Native Function (Network) ////////////////
	native boolean enableNetwork(byte[] filePath, int length);
	native boolean enableNetwork(byte[] MacAddress, byte[] filePath, int addrLength, int pathLength);
	native void disableNetwork();
	native void restartNetwork(byte[] filePath, int length);
	native int readNetworkState();
	native int readNetworkDeviceNumber();

    /////////////////// Native Function (Motor) ////////////////
	native void OperationMode(int motor1Mode, int motor2Mode);  
	// ��� ����ÿ��� disableMotor �� �Ŀ� ���  ...  EnableMotor(0, 0) << �� �Ҹ� �� �� 
	// ���� 1 - �ӵ� ���, 2 - ��ġ ���, 3 - ��ġ_�ӵ� ��� �� ����
	native void enableMotor(boolean Motor1EnOrDisable, boolean Motor2EnOrDisable);
	/*
	native void MotorVelocity(int Motor1Velocity, int Motor2Velocity);
	// �ӵ� ���� ������ 0 ~ 3000 rpm
	native void MotorPositionVelocity(int Motor1Position, int Motor2Position, int Motor1Velocity, int Motor2Velocity);
	// ���� ��ġ�� 12�޽��� 1ȸ�� (���� 100���ӱ�� ���Ͽ� 1ȸ�� 1200 �޽�) / �ӵ� ������ 0~3000
	native int[] readMotorVelocity();
	// �ӷ��� �����Ѵ�. [1] = ����1 [2] = ����2
	native int[] readMotorPosition();
	// ��ġ �޽��� �����Ѵ�. [1] = ����1 [2] = ����2
	native int[] readMotorCurrent();
	// ���Ϳ��� �Һ�� ���� ���� �����Ѵ�. [1] = ����1 [2] = ����2
	native int[] readMotorActualData();
	// ������ ��ġ �޽� �� �ӷ��� �����Ѵ�. [1] = ����1 ��ġ�޽� [2] = ����2 ��ġ�޽� [3] = ����1 �ӷ� [4] = ����2 �ӷ�
	native void StopDrive();
	// �κ� 2�� ������ ���� ���������� ���õǰ� �ٷ� ����
	*/
	
	/////////////////// Native Function (Network) ////////////////
	native void enablePsd(boolean psdEnOrDisable);
	native int[] getPsdDataClockwise();           // [0]~[8] ���� ������, [9] ���� �����͸� �д� ����  1 : �ð���� 0: �ݽð� ����
	native int[] getPsdDataAnticlockwise();       // [0]~[8] ���� ������, [9] ���� �����͸� �д� ����  1 : �ݽð���� 0: �ð� ����
	// ���� �����ʹ� 0~160cm ������ �Ÿ��� ��Ÿ����. ���D�� �� 7���� ���̹Ƿ� 0~6���������� �ùٸ� �����̴�.
    
	
	/////////////////// Native Function (driveRobot) ////////////////

	native void velocityDrive(float rpmBasedVelocity);      // RPM ���� �κ� �ӵ� ����(0~30)
	native void velocityRotate(float _rpmBasedRotation);           // RPM ���� ȸ�� �ӵ� ����(0~30)
	native void translationVelocity(float mpBasedVelocity);      // m/s ���� ������ �ӵ� ���� (MPS�� ��� 0~0.35)
	native void rotationVelocity(float mpBasedRotation);         // m/s ���� ȸ�� �ӵ� ����(MPS�� ��� 0~0.35)
	native void kinematicsDrive(float tv, float rv, int distanceMode);   // m/s ���� ȸ�� �� �ӵ� ���� (MPS�� ��� 0~0.35)
	native float[] readKinematicsDrive(float[] getedTvRv, int distanceMode);       // [1] m/s ���� ������ �ӵ� [2] ȸ�� �ӵ� ȹ�� (mode �� m/s(MP), mm/s(MMP)) 
	native void stopDrive();
}
