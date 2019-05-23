
#include "ofxAutoReloadedShader.h"



ofxAutoReloadedShader::ofxAutoReloadedShader() : type(TYPE::other), bWatchingFiles(false) {}

ofxAutoReloadedShader::~ofxAutoReloadedShader()
{
	disableWatchFiles();
}

bool ofxAutoReloadedShader::load(const std::filesystem::path& shaderName)
{
	return load(shaderName.string() + ".vert", shaderName.string() + ".frag", shaderName.string() + ".geom");
}

bool ofxAutoReloadedShader::load(const std::filesystem::path& vertName, const std::filesystem::path& fragName, const std::filesystem::path& geomName)
{
	unload();

	ofShader::setGeometryOutputCount(geometryOutputCount);
	ofShader::setGeometryInputType(geometryInputType);
	ofShader::setGeometryOutputType(geometryOutputType);

	// hackety hack, clear errors or shader will fail to compile
	GLuint err = glGetError();

	lastTimeCheckMillis = ofGetElapsedTimeMillis();
	setMillisBetweenFileCheck(2 * 1000);
	enableWatchFiles();

	loadShaderNextFrame = false;

	vertexShaderFilename = vertName;
	fragmentShaderFilename = fragName;
	geometryShaderFilename = geomName;

	vertexShaderFile.clear();
	fragmentShaderFile.clear();
	geometryShaderFile.clear();

	vertexShaderFile = ofFile(ofToDataPath(vertexShaderFilename));
	fragmentShaderFile = ofFile(ofToDataPath(fragmentShaderFilename));
	geometryShaderFile = ofFile(ofToDataPath(geometryShaderFilename));

	fileChangedTimes.clear();
	fileChangedTimes.push_back(getLastModified(vertexShaderFile));
	fileChangedTimes.push_back(getLastModified(fragmentShaderFile));
	fileChangedTimes.push_back(getLastModified(geometryShaderFile));

	if (!vertexShaderFilename.empty())
		setupShaderFromFile(GL_VERTEX_SHADER, vertexShaderFilename);

	if (!fragmentShaderFilename.empty())
		setupShaderFromFile(GL_FRAGMENT_SHADER, fragmentShaderFilename);

#ifndef TARGET_OPENGLES
	if (!geometryShaderFilename.empty())
		setupShaderFromFile(GL_GEOMETRY_SHADER_EXT, geometryShaderFilename);
#endif

	bindDefaults();

	return linkProgram();
}

bool ofxAutoReloadedShader::loadCompute(const std::filesystem::path& shaderName)
{
	type = TYPE::compute;

	lastTimeCheckMillis = ofGetElapsedTimeMillis();
	setMillisBetweenFileCheck(2 * 1000);
	enableWatchFiles();

	loadShaderNextFrame = false;

	computeShaderFilename = shaderName.string();
	computeShaderFile.clear();
	computeShaderFile = ofFile(ofToDataPath(computeShaderFilename));

	fileChangedTimes.clear();
	fileChangedTimes.push_back(getLastModified(computeShaderFile));

	return ofShader::loadCompute(shaderName);
}

void ofxAutoReloadedShader::_update(ofEventArgs &e)
{
	if (loadShaderNextFrame)
	{
		reloadShaders();
		loadShaderNextFrame = false;
	}

	int currTime = ofGetElapsedTimeMillis();

	if (((currTime - lastTimeCheckMillis) > millisBetweenFileCheck) &&
		!loadShaderNextFrame)
	{
		if (filesChanged())
			loadShaderNextFrame = true;

		lastTimeCheckMillis = currTime;
	}
}

bool ofxAutoReloadedShader::reloadShaders()
{
	if (type == TYPE::compute)
		return loadCompute(computeShaderFilename);
	else
		return load(vertexShaderFilename, fragmentShaderFilename, geometryShaderFilename);
}

void ofxAutoReloadedShader::enableWatchFiles()
{
	if (!bWatchingFiles)
	{
		ofAddListener(ofEvents().update, this, &ofxAutoReloadedShader::_update);
		bWatchingFiles = true;
	}
}

void ofxAutoReloadedShader::disableWatchFiles()
{
	if (bWatchingFiles)
	{
		ofRemoveListener(ofEvents().update, this, &ofxAutoReloadedShader::_update);
		bWatchingFiles = false;
	}
}

bool ofxAutoReloadedShader::filesChanged()
{
	bool fileChanged = false;

	if (vertexShaderFile.exists())
	{
		std::time_t vertexShaderFileLastChangeTime = getLastModified(vertexShaderFile);
		if (vertexShaderFileLastChangeTime != fileChangedTimes.at(0))
		{
			fileChangedTimes.at(0) = vertexShaderFileLastChangeTime;
			fileChanged = true;
		}
	}

	if (fragmentShaderFile.exists())
	{
		std::time_t fragmentShaderFileLastChangeTime = getLastModified(fragmentShaderFile);
		if (fragmentShaderFileLastChangeTime != fileChangedTimes.at(1))
		{
			fileChangedTimes.at(1) = fragmentShaderFileLastChangeTime;
			fileChanged = true;
		}
	}


	if (geometryShaderFile.exists())
	{
		std::time_t geometryShaderFileLastChangeTime = getLastModified(geometryShaderFile);
		if (geometryShaderFileLastChangeTime != fileChangedTimes.at(2))
		{
			fileChangedTimes.at(2) = geometryShaderFileLastChangeTime;
			fileChanged = true;
		}
	}


	if (computeShaderFile.exists() && type == TYPE::compute)
	{
		std::time_t computeShaderFileLastChangeTime = getLastModified(computeShaderFile);
		if (computeShaderFileLastChangeTime != fileChangedTimes.at(0))
		{
			fileChangedTimes.at(0) = computeShaderFileLastChangeTime;
			fileChanged = true;
		}
	}

	return fileChanged;
}

std::time_t ofxAutoReloadedShader::getLastModified(ofFile& _file)
{
	if (_file.exists())
		return std::filesystem::last_write_time(_file.path());
	else
		return 0;
}

void ofxAutoReloadedShader::setMillisBetweenFileCheck(int _millis)
{
	millisBetweenFileCheck = _millis;
}

void ofxAutoReloadedShader::setGeometryInputType(GLenum type) 
{
	ofShader::setGeometryInputType(type);
	geometryInputType = type;
}

void ofxAutoReloadedShader::setGeometryOutputType(GLenum type) 
{
	ofShader::setGeometryOutputType(type);
	geometryOutputType = type;
}

void ofxAutoReloadedShader::setGeometryOutputCount(int count) 
{
	ofShader::setGeometryOutputCount(count);
	geometryOutputCount = count;
}
