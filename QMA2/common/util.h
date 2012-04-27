/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2010-2012  hkrn                                    */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the MMDAI project team nor the names of     */
/*   its contributors may be used to endorse or promote products     */
/*   derived from this software without specific prior written       */
/*   permission.                                                     */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

#ifndef UTIL_H
#define UTIL_H

#include <QtGui/QtGui>
#include <vpvl2/vpvl2.h>

#if !defined(NDEBUG) && !defined(QMA_DEBUG)
#include <vpvl2/pmd/Bone.h>
#include <vpvl2/pmd/Model.h>
#include <vpvl2/pmd/Morph.h>
#include <vpvl2/pmx/Bone.h>
#include <vpvl2/pmx/Joint.h>
#include <vpvl2/pmx/Label.h>
#include <vpvl2/pmx/Material.h>
#include <vpvl2/pmx/Model.h>
#include <vpvl2/pmx/Morph.h>
#include <vpvl2/pmx/RigidBody.h>
#include <vpvl2/pmx/Vertex.h>
#include <vpvl2/vmd/BoneAnimation.h>
#include <vpvl2/vmd/BoneKeyframe.h>
#include <vpvl2/vmd/CameraAnimation.h>
#include <vpvl2/vmd/CameraKeyframe.h>
#include <vpvl2/vmd/LightAnimation.h>
#include <vpvl2/vmd/LightKeyframe.h>
#include <vpvl2/vmd/MorphAnimation.h>
#include <vpvl2/vmd/MorphKeyframe.h>
#include <vpvl2/vmd/Motion.h>
#define QMA_DEBUG
#endif

namespace internal
{

using namespace vpvl2;

const static Vector3 kWorldAabbSize(10000, 10000, 10000);

class String : public IString {
public:
    explicit String(const QString &s, IString::Codec codec = IString::kUTF8)
        : m_bytes(s.toUtf8()),
          m_value(s),
          m_codec(codec)
    {
    }
    ~String() {
    }

    bool startsWith(const IString *value) const {
        return m_value.startsWith(static_cast<const String *>(value)->m_value);
    }
    bool contains(const IString *value) const {
        return m_value.contains(static_cast<const String *>(value)->m_value);
    }
    bool endsWith(const IString *value) const {
        return m_value.endsWith(static_cast<const String *>(value)->m_value);
    }
    IString *clone() const {
        return new String(m_value);
    }
    const HashString toHashString() const {
        return HashString(m_bytes.constData());
    }
    bool equals(const IString *value) const {
        return m_value == static_cast<const String *>(value)->value();
    }
    const QString &value() const {
        return m_value;
    }
    const uint8_t *toByteArray() const {
        return reinterpret_cast<const uint8_t *>(m_bytes.constData());
    }
    size_t length() const {
        return m_bytes.length();
    }

private:
    const QByteArray m_bytes;
    const QString m_value;
    const IString::Codec m_codec;
};

class Encoding : public IEncoding {
public:
    Encoding()
        : m_sjis(QTextCodec::codecForName("Shift-JIS")),
          m_utf8(QTextCodec::codecForName("UTF-8")),
          m_utf16(QTextCodec::codecForName("UTF-16"))
    {
    }
    ~Encoding() {
    }

    const IString *stringConstant(ConstantType value) const {
        switch (value) {
        case kLeft: {
            static const String s("左");
            return &s;
        }
        case kRight: {
            static const String s("右");
            return &s;
        }
        case kFinger: {
            static const String s("指");
            return &s;
        }
        case kElbow: {
            static const String s("ひじ");
            return &s;
        }
        case kArm: {
            static const String s("腕");
            return &s;
        }
        case kWrist: {
            static const String s("手首");
            return &s;
        }
        default: {
            static const String s("");
            return &s;
        }
        }
    }
    IString *toString(const uint8_t *value, size_t size, IString::Codec codec) const {
        IString *s = 0;
        const char *str = reinterpret_cast<const char *>(value);
        switch (codec) {
        case IString::kShiftJIS:
            s = new String(m_sjis->toUnicode(str, size), codec);
            break;
        case IString::kUTF8:
            s = new String(m_utf8->toUnicode(str, size), codec);
            break;
        case IString::kUTF16:
            s = new String(m_utf16->toUnicode(str, size), codec);
            break;
        }
        return s;
    }
    IString *toString(const uint8_t *value, IString::Codec codec, size_t maxlen) const {
        size_t size = qstrnlen(reinterpret_cast<const char *>(value), maxlen);
        return toString(value, size, codec);
    }
    uint8_t *toByteArray(const IString *value, IString::Codec codec) const {
        const String *s = static_cast<const String *>(value);
        QByteArray bytes;
        switch (codec) {
        case IString::kShiftJIS:
            bytes = m_sjis->fromUnicode(s->value());
            break;
        case IString::kUTF8:
            bytes = m_utf8->fromUnicode(s->value());
            break;
        case IString::kUTF16:
            bytes = m_utf16->fromUnicode(s->value());
            break;
        }
        size_t size = bytes.length();
        uint8_t *data = new uint8_t[size + 1];
        memcpy(data, bytes.constData(), size);
        data[size] = 0;
        return data;
    }
    void disposeByteArray(uint8_t *value) const {
        delete[] value;
    }

private:
    const QTextCodec *m_sjis;
    const QTextCodec *m_utf8;
    const QTextCodec *m_utf16;
};

static inline QTextCodec *getTextCodec()
{
    static QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");
    return codec;
}

static inline const QString &noneString()
{
    static const QString none = QApplication::tr("(none)");
    return none;
}

static inline const QByteArray fromQString(const QString &value)
{
    return getTextCodec()->fromUnicode(value);
}

static inline const QString toQString(const uint8_t *value)
{
    return getTextCodec()->toUnicode(reinterpret_cast<const char *>(value));
}

static inline const QString toQString(const IString *value)
{
    return value ? static_cast<const internal::String *>(value)->value() : noneString();
}

static inline const QString toQString(const IModel *value)
{
    return value ? internal::toQString(value->name()) : noneString();
}

static inline const QString toQString(const IMotion *value)
{
    return value ? internal::toQString(value->name()) : noneString();
}

static inline const QString toQString(const IBone *value)
{
    return value ? internal::toQString(value->name()) : noneString();
}

static inline const QString toQString(const IMorph *value)
{
    return value ? internal::toQString(value->name()) : noneString();
}

static inline const QString toQString(const IBoneKeyframe *value)
{
    return value ? internal::toQString(value->name()) : noneString();
}

static inline const QString toQString(const IMorphKeyframe *value)
{
    return value ? internal::toQString(value->name()) : noneString();
}

static inline void dumpBones(IModel *model)
{
    Array<IBone *> bones;
    model->getBones(bones);
    const int nbones = bones.count();
    for (int i = 0; i < nbones; i++) {
        IBone *bone = bones[i];
        const Transform &transform = bone->localTransform();
        const Vector3 &p = transform.getOrigin();
        const Quaternion &q = transform.getRotation();
        qDebug().nospace() << "index=" << i
                           << " name=" << internal::toQString(bone)
                           << " position=" << QVector3D(p.x(), p.y(), p.z())
                           << " rotation=" << QQuaternion(q.w(), q.x(), q.y(), q.z());
    }
}

static inline void dumpBoneKeyFrame(const IBoneKeyframe *frame, int index = 0)
{
    const Vector3 &p = frame->position();
    const Quaternion &q = frame->rotation();
    qDebug().nospace() << "index=" << index
                       << " frameIndex=" << frame->frameIndex()
                       << " name=" << internal::toQString(frame)
                       << " position=" << QVector3D(p.x(), p.y(), p.z())
                       << " rotation=" << QQuaternion(q.w(), q.x(), q.y(), q.z());
}

static inline void dumpBoneKeyFrames(const IMotion *motion)
{
    const int nframes = motion->countKeyframes(IKeyframe::kBone);
    for (int i = 0; i < nframes; i++)
        dumpBoneKeyFrame(motion->findBoneKeyframeAt(i), i);
}

static inline const QMatrix4x4 toMatrix4x4(float matrixf[16])
{
    qreal matrixd[16];
    for (int i = 0; i < 16; i++)
        matrixd[i] = matrixf[i];
    return QMatrix4x4(matrixd);
}

static inline const QString openFileDialog(const QString &name,
                                           const QString &desc,
                                           const QString &exts,
                                           QSettings *settings)
{
    /* ファイルが選択されている場合はファイルが格納されているディレクトリを指す絶対パスを設定に保存しておく */
    const QString &path = settings->value(name, QDir::homePath()).toString();
    const QString &fileName = QFileDialog::getOpenFileName(0, desc, path, exts);
    if (!fileName.isEmpty()) {
        QDir dir(fileName);
        dir.cdUp();
        settings->setValue(name, dir.absolutePath());
    }
    return fileName;
}

static const inline QString openSaveDialog(const QString &name,
                                           const QString &desc,
                                           const QString &exts,
                                           const QString &defaultFilename,
                                           QSettings *settings)
{
    const QDir base(settings->value(name, QDir::homePath()).toString());
    const QString &path = base.absoluteFilePath(defaultFilename);
    const QString &fileName = QFileDialog::getSaveFileName(0, desc, path, exts);
    if (!fileName.isEmpty()) {
        QDir dir(fileName);
        dir.cdUp();
        settings->setValue(name, dir.absolutePath());
    }
    return fileName;
}


}

#endif // UTIL_H
