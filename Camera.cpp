
#include "main.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
//										THE CCAMERA POSITION CAMERA
/////////////////////////////////////////////////////////////////////////////////////////////////
void CCamera::Position_Camera(float pos_x,  float pos_y,  float pos_z,
							  float view_x, float view_y, float view_z,
							  float up_x,   float up_y,   float up_z)
{
	mPos	= tVector3(pos_x,  pos_y,  pos_z ); // set position
	mView	= tVector3(view_x, view_y, view_z); // set view
	mUp		= tVector3(up_x,   up_y,   up_z  ); // set the up vector	
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//										THE CCAMERA MOVE CAMERA
/////////////////////////////////////////////////////////////////////////////////////////////////
void CCamera::Move_Camera(float speed)
{
	tVector3 vVector = mView - mPos;	// Get the view vector
	
	// forward positive camera speed and backward negative camera speed.
	mPos.x  = mPos.x  + vVector.x * speed;
	mPos.z  = mPos.z  + vVector.z * speed;
	mView.x = mView.x + vVector.x * speed;
	mView.z = mView.z + vVector.z * speed;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//										THE CCAMERA ROTATE VIEW
/////////////////////////////////////////////////////////////////////////////////////////////////
void CCamera::Rotate_View(float speed)
{
	tVector3 vVector = mView - mPos;	// Get the view vector

	mView.z = (float)(mPos.z + sin(speed)*vVector.x + cos(speed)*vVector.z);
	mView.x = (float)(mPos.x + cos(speed)*vVector.x - sin(speed)*vVector.z);
}




/////////////////////////////////////////////////////////////////////////////////////////////////
//										THE CCAMERA ROTATE POSITION
/////////////////////////////////////////////////////////////////////////////////////////////////
void CCamera::Rotate_Position(float speed)
{	
	tVector3 vVector = mPos - mView;

	mPos.z = (float)(mView.z + sin(speed)*vVector.x + cos(speed)*vVector.z);
	mPos.x = (float)(mView.x + cos(speed)*vVector.x - sin(speed)*vVector.z);
}



//NEW//////////////////NEW//////////////////NEW//////////////////NEW////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
//										THE CCAMERA STRAFE CAMERA
/////////////////////////////////////////////////////////////////////////////////////////////////
void CCamera::Strafe_Camera(float speed)
{
	tVector3 vVector = mView - mPos;	// Get the view vector
	tVector3 vOrthoVector;              // Orthogonal vector for the view vector

	vOrthoVector.x = -vVector.z;
	vOrthoVector.z =  vVector.x;

	// left positive cameraspeed and right negative -cameraspeed.
	mPos.x  = mPos.x  + vOrthoVector.x * speed;
	mPos.z  = mPos.z  + vOrthoVector.z * speed;
	mView.x = mView.x + vOrthoVector.x * speed;
	mView.z = mView.z + vOrthoVector.z * speed;
}

//NEW//////////////////NEW//////////////////NEW//////////////////NEW////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
//										THE CCAMERA MOUSE MOVE
/////////////////////////////////////////////////////////////////////////////////////////////////
void CCamera::Mouse_Move( int wndWidth, int wndHeight )
{
	POINT mousePos;	
	int mid_x = wndWidth  >> 1;	
	int mid_y = wndHeight >> 1;	
	float angle_y  = 0.0f;				
	float angle_z  = 0.0f;							
	
	GetCursorPos(&mousePos);	// Get the mouse cursor 2D x,y position					
	
	if( (mousePos.x == mid_x) && (mousePos.y == mid_y) ) return;

	SetCursorPos(mid_x, mid_y);	// Set the mouse cursor in the center of the window						

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angle_y = (float)( (mid_x - mousePos.x) ) / 1000;		
	angle_z = (float)( (mid_y - mousePos.y) ) / 1000;

	// The higher the value is the faster the camera looks around.
	mView.y += angle_z * 5;

	// limit the rotation around the x-axis
	if(mView.y > 15.0f)		mView.y = 6.0f;
	if(mView.y < 0.3f)		mView.y = 0.3f;
	
	Rotate_Position(-angle_y);
}



//Ronny André Reierstad
//www.morrowland.com
//apron@morrowland.com