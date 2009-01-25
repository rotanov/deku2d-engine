#ifndef _RES_H
#define _RES_H

#include "CoreUtils.h"
#include "OpenglUtils.h"
#include "xml.h"

//define resource types
#define CONFIG_FILE_NAME "config/configuration.xml"
#define CRESOURCE_SECTION_FONTS "font"
#define CRESOURCE_SECTION_TEXTURES "textures"

#define CRESOURCE_TYPE_FONT				0x01
#define CRESOURCE_TYPE_IMAGE			0x02
#define	CRESOURCE_TYPE_PARTICLESYSTEM	0x03
#define CRESOURCE_TYPE_ANIMATION		0x04
#define CRESOURCE_TYPE_XML				0x05

enum EResType {RT_FONT, RT_IMAGE, RT_PSYSTEM, RT_ANIMATION, RT_XML};


#define OBJ_SPRITE		0x01
#define OBJ_PSYSTEM		0x02
#define OBJ_FONT		0x03
#define	OBJ_USER_DEFINED 0x04
#define OBJ_TEXTURE_RES 0x05

class CFactory : public CObjectList
{
public:
	static			CFactory* Instance();
	void			FreeInst();
	CObject*		Create(int ObjectId, CreateFunc creator);
	bool			InitManagers(CUpdateManager *vUpdateManager, CRenderManager *vRenderManager);

protected:
	bool initialized;
	CFontManager *FontManager;
	CUpdateManager *UpdateManager;
	CRenderManager *RenderManager;
	CTextureManager *TextureManager;

	CFactory();
	~CFactory();
	static			CFactory* _instance;
	static int		_refcount;
};


class CResourceManager
{
public:
	string DataPath, TexturesFldr, FontsFldr;

	CResourceManager();	
	CObject* LoadResource(char *ResourceList, char* section, char *name, CreateFunc creator);
	bool LoadResources(char *ResourceList);
	bool LoadTextures(char *ResourceList);
private:
};

#endif _RES_H