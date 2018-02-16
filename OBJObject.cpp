/*
 * OBJ File Loader (to work specifically with exports from Maya 6.0)
 * (supporting both normals/texture mapping as well)
 *
 * Author: James McCrae
 */

#include "OBJObject.h"

Texture::Texture() :
    texindex(0),
    gl_isloaded(false)
{
}

void Texture::MakeReady()
{
    if (gl_isloaded) {
        return;
    }

    Load();
}

void Texture::Load()
{
    glGenTextures(1, &texindex);
    glBindTexture(GL_TEXTURE_2D, texindex);

    //no mipmap

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    glBindTexture(GL_TEXTURE_2D, 0);

    gl_isloaded = true;
}

void Texture::Unload()
{
    glDeleteTextures(1, &texindex);
    texindex = 0;
    gl_isloaded = false;

}

Material::Material() :
    tex(NULL)
{
}

OBJObject::OBJObject() {        

}

OBJObject::~OBJObject() {
      
}

void OBJObject::loadMTL(QString path)
{
    QFile mtlfile(path);
    if (!mtlfile.open( QIODevice::ReadOnly | QIODevice::Text )) {
        QMessageBox::critical(NULL, QString("Can't open '") + path + QString("' for read."), QString("Sorry."));
        return;
    }

    QTextStream mtlfs(&mtlfile);

    while (!mtlfs.atEnd()) {
        QStringList mtlline = mtlfs.readLine().split(" ");

        if (mtlline.first().compare("newmtl") == 0) {
            m_mats.push_back(Material());
            m_mats.last().name = mtlline.last();
            qDebug() << "OBJObject::loadMTL - adding material with name " << m_mats.last().name;
        }
        else if (mtlline.first().compare("kd", Qt::CaseInsensitive) == 0) {
            if (m_mats.isEmpty()) {
                 m_mats.push_back(Material());
            }
            m_mats.last().kd = new float[3];
            m_mats.last().kd[0] = mtlline[1].toFloat();
            m_mats.last().kd[1] = mtlline[2].toFloat();
            m_mats.last().kd[2] = mtlline[3].toFloat();

        }
        else if (mtlline.first().compare("ka", Qt::CaseInsensitive) == 0) {
            if (m_mats.isEmpty()) {
                 m_mats.push_back(Material());
            }
            m_mats.last().ka = new float[3];
            m_mats.last().ka[0] = mtlline[1].toFloat();
            m_mats.last().ka[1] = mtlline[2].toFloat();
            m_mats.last().ka[2] = mtlline[3].toFloat();

        }
        else if (mtlline.first().compare("map_kd", Qt::CaseInsensitive) == 0) {

            if (m_mats.isEmpty()) {
                 m_mats.push_back(Material());
            }
            m_mats.last().map_kd = mtlline.last();
            m_mats.last().tex = loadTexture(m_basePath + "/" + m_mats.last().map_kd);
            qDebug() << "OBJObject::loadMTL - texindex " << m_mats.last().tex->texindex << " assigned to texture " << m_mats.last().map_kd;
        }
    }

    mtlfile.close();
}

Texture * OBJObject::loadTexture(const QString & file)
{
    QFileInfo fi(file);
    QString my_canonical_path = fi.canonicalFilePath();

    //return existing texture object reference if it exists
    for (int i=0; i<all_textures.size(); ++i) {
        if (QString::compare(my_canonical_path, all_textures[i].canonical_path) == 0) {
            return &all_textures[i];
        }
    }

    return CreateTextureObject(my_canonical_path);
}

Texture * OBJObject::CreateTextureObject(const QString & file)
{
    //else generate new texture object, return reference to that
    all_textures.push_back(Texture());

    QImage buf;
    if (!buf.load(file)) {
        qDebug() << "OBJObject::CreateTextureObject - Couldn't load file " << file;
    }

    all_textures.last().canonical_path = file;
    all_textures.last().image = QGLWidget::convertToGLFormat( buf );

    //all_textures.last().Load(); //load on first use

    return &all_textures.last();
}

bool OBJObject::load(QString path) {
 
    //Load the OBJ file...
    //Format details:


    QFile infile(path);

    if (!infile.open( QIODevice::ReadOnly | QIODevice::Text )) {
        QMessageBox::critical(NULL, QString("Can't open '") + path + QString("' for read."), QString("Sorry."));
        return false;
    }

    //set base path (for loading resources in same dir)
    QFileInfo fi(path);
    m_basePath = fi.absolutePath();

    QTextStream ifs( &infile );

    int curMaterial = -1;

    while (!ifs.atEnd()) {

        QStringList line = ifs.readLine().split(" ");

        if (line.first().compare("mtllib") == 0) {

            QString mtlfilepath = m_basePath + "/" + line.last();
            loadMTL(mtlfilepath);

        }
        else if (line.first().compare("usemtl") == 0) {
            for (int i=0; i<m_mats.size(); ++i) {
                if (line.last().compare(m_mats[i].name) == 0) {
                    curMaterial = i;
                    break;
                }
            }
        }
        else if (line.first().compare("v") == 0) {
            m_verts.push_back(QVector3D(line[1].toFloat(), line[2].toFloat(), line[3].toFloat()));
        }
        else if (line.first().compare("vn") == 0) {
            m_norms.push_back(QVector3D(line[1].toFloat(), line[2].toFloat(), line[3].toFloat()));
        }
        else if (line.first().compare("vt") == 0) {
            m_uvs.push_back(QVector2D(line[1].toFloat(), line[2].toFloat()));
        }
        else if (line.first().compare("f") == 0) {

            Face f;

            for (int i=1; i<line.size(); ++i) {
                QStringList eachv = line[i].split("/");

                if (!eachv[0].isEmpty()) {
                    f.vi.push_back(eachv[0].toInt() - 1);
                }
                if (!eachv[1].isEmpty()) {
                    f.uvi.push_back(eachv[1].toInt() - 1);
                }
                if (!eachv[2].isEmpty()) {
                    f.ni.push_back(eachv[2].toInt() - 1);
                }
            }

            f.mati = curMaterial;
            m_faces.push_back(f);
        }
    }

    infile.close();

	glDisable(GL_TEXTURE_2D);

	return true;

}

void OBJObject::draw() {
  
    //ensure all textures are ready for draw
    for (int i=0; i<m_mats.size(); ++i) {
        if (m_mats[i].tex) {
            m_mats[i].tex->MakeReady();
        }
    }

//    if (disp_list > 0) {
//        glCallList(disp_list);
//        return;
//    }


//    disp_list = glGenLists(1);
//    glNewList(disp_list, GL_COMPILE_AND_EXECUTE);

    //perform draw
    int curMaterial = -1;

    //REFACTOR TODO - Move to more modern OpenGL - this draw is extremely slow!
    glBegin(GL_TRIANGLES);

    for (int i=0; i<m_faces.size(); ++i) {

        if (curMaterial != m_faces[i].mati) {

            glEnd();

            curMaterial = m_faces[i].mati;

            glColor3fv(m_mats[curMaterial].kd);

            if (m_mats[curMaterial].map_kd.isEmpty()) {

                glDisable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);

            }
            else {

                glColor3f(1, 1, 1);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, m_mats[curMaterial].tex->texindex);

            }

            glBegin(GL_TRIANGLES);

        }

        //tri fan out this poly (works well in case more than 3 verts per face)
        for (int j=1; j+1 < m_faces[i].vi.size(); ++j) {

            if (!m_faces[i].ni.isEmpty()) {
                const QVector3D & n1 = m_norms[m_faces[i].ni[0]];
                glNormal3f(n1[0], n1[1], n1[2]);
            }

            if (!m_faces[i].uvi.isEmpty()) {
                const QVector2D & uv1 = m_uvs[m_faces[i].uvi[0]];
                glTexCoord2f(uv1[0], uv1[1]);
            }

            const QVector3D & v1 = m_verts[m_faces[i].vi[0]];
            glVertex3f(v1[0], v1[1], v1[2]);

            if (!m_faces[i].ni.isEmpty()) {
                const QVector3D & n2 = m_norms[m_faces[i].ni[j]];
                glNormal3f(n2[0], n2[1], n2[2]);
            }

            if (!m_faces[i].uvi.isEmpty()) {
                const QVector2D & uv2 = m_uvs[m_faces[i].uvi[j]];
                glTexCoord2f(uv2[0], uv2[1]);
            }

            const QVector3D & v2 = m_verts[m_faces[i].vi[j]];
            glVertex3f(v2[0], v2[1], v2[2]);

            if (!m_faces[i].ni.isEmpty()) {
                const QVector3D & n3 = m_norms[m_faces[i].ni[j+1]];
                glNormal3f(n3[0], n3[1], n3[2]);
            }

            if (!m_faces[i].uvi.isEmpty()) {
                const QVector2D & uv3 = m_uvs[m_faces[i].uvi[j+1]];
                glTexCoord2f(uv3[0], uv3[1]);
            }

            const QVector3D & v3 = m_verts[m_faces[i].vi[j+1]];
            glVertex3f(v3[0], v3[1], v3[2]);
        }
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glColor3f(1, 1, 1);

//    glEndList();
    
}

QVector <QVector3D> & OBJObject::getVerts() {
    return m_verts;
}
    
void OBJObject::setVerts(const QVector<QVector3D> verts) {
    m_verts = verts;
}
