/*
 * Header file for OBJ file loader/displayer.
 *
 * Author: James McCrae
 */

#ifndef _OBJObject
#define _OBJObject

#include <QtOpenGL>
#include <QtCore>

struct Face {
    QVector <int> vi;
    QVector <int> ni;
    QVector <int> uvi;

    int mati;
};

struct Texture {

    Texture();
    void MakeReady();
    void Load();
    void Unload();

    QString canonical_path;
    QImage image;
    GLuint texindex;
    bool gl_isloaded;

};

struct Material {

    Material();

    QString name;
    GLfloat * kd;
    GLfloat * ka;
    QString map_kd;
    Texture * tex;

};

class OBJObject : public QObject {
    Q_OBJECT

public:
    
    OBJObject();
    ~OBJObject();
    bool load(QString path);
    void draw();

    QVector <QVector3D> & getVerts();
    void setVerts(const QVector <QVector3D> verts);

private:

    void loadMTL(QString path);
    Texture *  loadTexture(const QString & file);
    Texture * CreateTextureObject(const QString & file);
    
    QString m_basePath;

    QVector <QVector3D> m_verts;
    QVector <QVector3D> m_norms;
    QVector <QVector2D> m_uvs;

    QVector <Face> m_faces;

    QVector <Material> m_mats;

    QList <Texture> all_textures;

    GLuint disp_list;
    
};

#endif
