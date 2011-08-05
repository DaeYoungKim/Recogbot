package dev.imagelab.gallaxyfacedetection;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.Paint.Style;
import android.graphics.Path.Direction;
import android.media.FaceDetector;
import android.media.FaceDetector.Face;
import android.util.Log;
import android.view.View;

public class FaceDetectionView extends View{

	private FaceDetector.Face[] faces;     // �νĵ� ���� ������ ����� ��
	private Rect TopView;                  // 320*240 �������� ķó �̹����� �׸� �� ����
	private FaceDetector detector;         // Face Detector ����
	public Bitmap bitmap;                 // ĸó �̹����� ����� ���Դϴ�.
	
	private final Paint paint;             // �����̰��~
	private final String SERVERIP;         // �ڽ��� PC IP �Դϴ�.
	private final int SERVERPORT;          // ��Ʈ�Դϴ�. ������ 9889�̴� 9889�� ���ּ���
	
	public FaceDetectionView(Context context) {
		super(context);
		SERVERIP = "192.168.10.104";        
		SERVERPORT = 9889;                   
		
		faces = new FaceDetector.Face[5];   // �ִ� �� �ν� ������ 5��
		TopView = new Rect(0, 0, 640, 480);   // �̹����� ������ �� ������� �׷��� 320*240
		
		paint = new Paint();
	    paint.setColor(Color.argb(255, 255, 0, 0)); 
	    paint.setStyle(Style.STROKE); 
	    paint.setAntiAlias(true);
	    paint.setStrokeWidth(3); 
	    
		SocketCamera CapImage = new SocketCamera(SERVERIP, SERVERPORT);
		if(!CapImage.open()){Log.d("TCP", "Failed to obtain image over network");}
		bitmap = CapImage.capture(); //capture the frame onto the canvas
		
		// ������ ����!
		Log.i("LOG_TAG", "build faces...."); 
	    detector = new FaceDetector(bitmap.getWidth(), bitmap.getHeight(), faces.length);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		super.onDraw(canvas);
		
		SocketCamera CapImage = new SocketCamera(SERVERIP, SERVERPORT);
		if(!CapImage.open()){Log.d("TCP", "Failed to obtain image over network");}
		bitmap = CapImage.capture(); //capture the frame onto the canvas
		CapImage.close();
		
		Log.i("information", "Bitmap Config Change!!");

		bitmap = bitmap.copy(Bitmap.Config.RGB_565, true);   
		canvas.drawBitmap(bitmap, null, TopView, paint);     
		
		int numFaces = detector.findFaces(bitmap, faces);    
		Log.i("information", "faces num : " + numFaces); 
		
		if (numFaces > 0) {						// ���� �ϳ��� ���̶� ������ �ȴٸ�	    
		    for (int i = 0; i < numFaces; i++) {
		        Face face = faces[i];          
		        PointF midPoint = new PointF(0, 0);  
		        face.getMidPoint(midPoint); 	// �� �� ������ ��� ��
		        float eyesDistance = face.eyesDistance(); 	// �� �� ������ �Ÿ�
		       
		        Path circle;					// �� ��ġ�� ���׶�̸� �׷��ݴϴ�.
		        	
		        circle = new Path();
		        circle.addCircle(midPoint.x*2, (midPoint.y+eyesDistance/2)*2, (eyesDistance*1.7F)*2, Direction.CW);
		        canvas.drawPath(circle, paint);                
		    }
		}
		invalidate();     // call onDraw(in UI thread)
	}
}
