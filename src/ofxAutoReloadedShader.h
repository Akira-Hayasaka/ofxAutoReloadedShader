#pragma once

#include "ofMain.h"

class ofxAutoReloadedShader : public ofShader
{
	
  public:
	ofxAutoReloadedShader();
	~ofxAutoReloadedShader();
	
	// override the initialisation functions
	bool load(string shaderName );
	bool load(string vertName, string fragName, string geomName);
	bool loadCompute(const std::filesystem::path& shaderName);

	bool reloadShaders();
	
	void enableWatchFiles();
	void disableWatchFiles();
		
	void setMillisBetweenFileCheck( int _millis );
	
	void _update(ofEventArgs &e);
    
    void setGeometryInputType(GLenum type);
    void setGeometryOutputType(GLenum type);
    void setGeometryOutputCount(int count);
	
private:
	
	bool bWatchingFiles;
	bool filesChanged();
	
	bool loadShaderNextFrame;
	
	std::time_t getLastModified( ofFile& _file );

	enum struct TYPE { compute, other }; TYPE type;
	
	int lastTimeCheckMillis;
	int millisBetweenFileCheck;
	
	string vertexShaderFilename;
	string fragmentShaderFilename;
	string geometryShaderFilename;
	string computeShaderFilename;
	
	ofFile vertexShaderFile;
	ofFile fragmentShaderFile;
	ofFile geometryShaderFile;
	ofFile computeShaderFile;
	
	vector< std::time_t > fileChangedTimes;
    
    GLenum geometryInputType, geometryOutputType;
    int geometryOutputCount;
};


