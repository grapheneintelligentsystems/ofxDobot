#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

typedef struct TagPose {

	float x = 0;
	float y = 0;
	float z = 0;
	float r = 0;
	float jointAngle[4] = { 0,0,0,0 };

}Pose;

typedef struct tagHOMEParams {
	float x; 
	float y;
	float z; 
	float r;
} HOMEParams;

typedef struct tagJOGJointParams {
	float velocity[4];//Joint velocity of 4 axis
	float acceleration[4]; //Joint acceleration of 4 axis
}JOGJointParams;

typedef struct tagJOGCoordinateParams {
	float velocity[4];//Coornite velocity of 4 axis(x,y,z,r)
	float acceleration[4];//Coordinate acceleration of 4 zxis(x,y,z,r)
} JOGCoordinateParams;

typedef struct tagJOGCommonParams {
	float velocityRatio;//Velocity ratio,share joint jog and coordinated jog
	float accelerationRatio; //Acceleration ratio,share joint jog and coordinated jog
} JOGCommonParams;

typedef struct tagJOGCmd {
	uint8_t isJoint;//Jog mode 0-coordinate jog 1-Joint jog
	uint8_t cmd;//Jog command(Value range0~8)
}JOGCmd;

typedef struct tagCPParams {
	float planAcc; // Maximum value of planned acceleration
	float junctionVel; // Maximum value of junction acceleration
	union {
		float acc; //Maximum value of actual acceleration,using in non-real-time mode
		float period; //Interpolation cycle, real-time mode
	};
	uint8_t realTimeTrack; //0�non real time mode; 1�non real time mode
} CPParams;

typedef struct tagCPCmd {
	uint8_t cpMode; //CP mode 0-relative mode 1-absolute mode
	float x; //x-coordinate increment / x coordinate
	float y; //y-coordinate increment / y coordinate
	float z; //z-coordinate increment/ z coordinate
	union {
		float velocity; // Reserved
		float power; //Laser power
	};
} CPCmd;

typedef struct tagWAITCmd {
	uint32_t timeout; //Unit ms
} WAITCmd;

typedef struct tagArmAngleError {

	float rearArmAngleError;
	float frontArmAngleError;

} ArmAngleError;

enum ptpMode{ JUMP_XYZ, MOVJ_XYZ, MOVL_XYZ, JUMP_ANGLE, MOVJ_ANGLE, MOVL_ANGLE, MOVJ_INC, MOVL_INC, MOVJ_XYZ_INC, JUMP_MOVL_XYZ };

enum {
	IDEL, //Void
	AP_DOWN, // X+/Joint1+
	AN_DOWN, // X-/Joint1-
	BP_DOWN, // Y+/Joint2+
	BN_DOWN, // Y-/Joint2-
	CP_DOWN, // Z+/Joint3+
	CN_DOWN, // Z-/Joint3-
	DP_DOWN, // R+/Joint4+
	DN_DOWN // R-/Joint4-
};

class ofxDobot : public ofThread {

public:

	ofxDobot();
	bool setup(string serialName);

	bool load(string fileName);
	void play();
	void stop();
	void clear();


	string getDeviceSN();

	void resetPose(uint8_t manual, float rearArmAngle, float frontArmAngle);
	Pose getPose();

	void getAlarmsState(uint8_t alarmsState[16]);
	void clearAllAlarmsState();

	void setHomeCmd();
	void setHomeParams(bool isQueue, HOMEParams params);

	//void getHomeParams()

	void setPTPJointParams(bool isQueued, float velocity[4], float acceleration[4]);
	void setPTPCoordinateParams(bool isQueued, float xyzVelocity, float rVelocity, float xyzAcceleration, float rAcceleration);
	void setPTPJumpParams(bool isQueued, float jumpHeight, float zLimit);
	void setPTPCommonParams(bool isQueued, float velocityRation, float accelerationRatio);
	void setPTPCmd(ptpMode mode, float x, float y, float z, float r);

	void setCPParams(bool isQueue, CPParams params);
	CPParams getCPParams();
	void setCPCmd(CPCmd cmd);

	void setJOGJointParams(bool isQueue, JOGJointParams params);
	JOGJointParams getJOGJointParams();
	void setJOGCoordinateParams(bool isQueue, JOGCoordinateParams params);
	JOGCoordinateParams getJOGCoordinateParams();
	void setJOGCommonParams(bool isQueue, JOGCommonParams params);
	JOGCommonParams getJOGCommonParams();
	void setJOGCmd(JOGCmd cmd);

	void setWAITCmd(WAITCmd cmd);

	void setQueuedCmdStartExec();
	void setQueuedCmdStopExec();
	void setQueuedCmdClear();
	int getQueuedCmdCurrentIndex();
	int getQueuedCmdLeftSpace();


	//Calibration

	void setAngleSensorStaticError(ArmAngleError armAngleError);
	ArmAngleError getAngleSensorStaticError();

	

private:

	void threadedFunction();

	bool connected;
	string serialName;

	ofSerial serial;

	Pose pose;
	
	Pose nullPose;

	JOGJointParams jogJointParams;
	JOGCoordinateParams jogCoordinateParams;
	JOGCommonParams	jogCommonParams;
	CPParams	cpParams;

	ArmAngleError armAngleError;

	bool waitingMessage;

	uint64_t queuedCmdIndex;

	uint32_t queuedLeftSpace;

	enum protocolFunction{ DeviceSN, DeviceName, DeviceVersion, GetPose = 10, ResetPose, GetAlarms = 20, ClearAllAlarmsState, HomeParams = 30, HomeCmd, HHTTrigMode = 40, HHTrigOutputEnabled,
		HHTTrigOutput, ArmOrientation = 50, EndEffectorParams = 60, EndEffectorLaser,  EndEffectorSuctionCup, EndEffectorGripper, SETGETJOGJointParams = 70, SETGETJOGCoordinateParams, SETGETJOGCommonParams , SetJOGCmd,
		PTPJointParams = 80, PTPCoordinateParams, PTPJumpParams, PTPCommonParams, PTPCmd, SETGETCPParams = 90, SetCPCmd, SetCPLECmd, ARCParams = 100, ARCCmd, SetWAITCmd = 110, TRIGCmd = 120, IOMultiplexing = 130,
		IODO, IOPWM, GetIODI, GetIOADC, SetEMotor, AngleSensorStaticError = 140,  WIFIConfigMode = 150, WIFISSID, WIFIPassword, WIFIIPAddress, WIFINetmask, WIFIGateway, WIFIDNS, GetWIFIConnectStatus, SetQueuedCmdStartExec = 240, 
		SetQueuedCmdStopExec, SetQueueCmdForceStopExec, SetQueuedCmdStartDownload, SetQueuedCmdStopDownload, SetQueuedCmdClear, GetQueuedCmdCurrentIndex, GetQueuedCmdLeftSpace
	};

	
	string noConnection;

	uint8_t *alarmsState;



};
