#pragma once

#include "ofMain.h"

class ofxAutoReloadedShader : public ofShader
{

public:
	ofxAutoReloadedShader();
	~ofxAutoReloadedShader();

	bool load(const std::filesystem::path& shaderName);
	bool load(const std::filesystem::path& vertName, const std::filesystem::path& fragName, const std::filesystem::path& geomName = "");
	bool loadCompute(const std::filesystem::path& shaderName);
	bool reloadShaders();
	void enableWatchFiles();
	void disableWatchFiles();
	void setMillisBetweenFileCheck(int _millis);
	void _update(ofEventArgs &e);
	void setGeometryInputType(GLenum type);
	void setGeometryOutputType(GLenum type);
	void setGeometryOutputCount(int count);

private:

	bool filesChanged();
	std::time_t getLastModified(ofFile& _file);

	enum struct TYPE { compute, other }; TYPE type;

	bool bWatchingFiles;
	bool loadShaderNextFrame;
	int lastTimeCheckMillis;
	int millisBetweenFileCheck;

	vector< std::time_t > fileChangedTimes;
	std::filesystem::path vertexShaderFilename;
	std::filesystem::path fragmentShaderFilename;
	std::filesystem::path geometryShaderFilename;
	std::filesystem::path computeShaderFilename;
	ofFile vertexShaderFile;
	ofFile fragmentShaderFile;
	ofFile geometryShaderFile;
	ofFile computeShaderFile;

	GLenum geometryInputType, geometryOutputType;
	int geometryOutputCount;
};


