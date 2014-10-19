/*
 * ShadowMap.h
 *
 *  Created on: 17/10/2014
 *      Author: broadconn
 */

#ifndef SHADOWMAP_H_
#define SHADOWMAP_H_

namespace std {

class ShadowMapFBO
{
    public:
        ShadowMapFBO();

        ~ShadowMapFBO();

        bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

        void BindForWriting();

        void BindForReading(GLenum TextureUnit);

    private:
        GLuint m_fbo;
        GLuint m_shadowMap;
};

} /* namespace std */

#endif /* SHADOWMAP_H_ */
