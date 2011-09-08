#ifndef _2DE_DEBUG_BOX_H_
#define _2DE_DEBUG_BOX_H_

#include "2de_Core.h"
#include "2de_RenderableComponent.h"
#include "2de_GraphicsLow.h"

namespace Deku2D
{
 class CDebugBoxComponent : public CRenderableComponent
        {
        public:
                D2D_DECLARE_VISITABLE()


                CDebugBoxComponent()
                {
                        CRenderConfig NewConfiguration = GetConfiguration();
                        NewConfiguration.doIgnoreTransform = true;
                        SetConfiguration(NewConfiguration);
                }
                virtual ~CDebugBoxComponent(){}
        };

}	//	namespace Deku2D

#endif
