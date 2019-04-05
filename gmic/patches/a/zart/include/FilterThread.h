/** -*- mode: c++ ; c-basic-offset: 2 -*-
 * @file   FilterThread.h
 * @author Sebastien Fourey
 * @date   July 2010
 * @brief  Declaration of the class ImageFilter
 *
 * This file is part of the ZArt software's source code.
 *
 * Copyright Sebastien Fourey / GREYC Ensicaen (2010-...)
 *
 *                    https://foureys.users.greyc.fr/
 *
 * This software is a computer program whose purpose is to demonstrate
 * the possibilities of the GMIC image processing language by offering the
 * choice of several manipulations on a video stream aquired from a webcam. In
 * other words, ZArt is a GUI for G'MIC real-time manipulations on the output
 * of a webcam.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info". See also the directory "Licence" which comes
 * with this source code for the full text of the CeCILL license.
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 *
 */
#ifndef _FILTERTHREAD_H_
#define _FILTERTHREAD_H_

#include <QThread>
#include "Common.h"
#include "CriticalRef.h"
#include "gmic.h"
class ImageSource;
class QMutex;
class QImage;
class QSemaphore;

class FilterThread : public QThread {
  Q_OBJECT
public:
  enum PreviewMode
  {
    Full,
    TopHalf,
    LeftHalf,
    BottomHalf,
    RightHalf,
    DuplicateVertical,
    DuplicateHorizontal,
    Original
  };

  FilterThread(ImageSource & webcam, const QString & command, QImage * outputImageA, QMutex * imageMutexA, QImage * outputImageB, QMutex * imageMutexB, PreviewMode previewMode, int frameSkip, int fps,
               QSemaphore * blockingSemaphore);

  virtual ~FilterThread();

  void run();
  void setMousePosition(int x, int y, int buttons);

  void setArguments(const QString &);

public slots:

  void setFrameSkip(int);
  void setFPS(int);
  void setPreviewMode(PreviewMode);
  void stop();
  void setViewSize(const QSize &);

signals:

  void imageAvailable();
  void endOfCapture();

private:
  void setCommand(const QString & command);

  ImageSource & _imageSource;
  QString _command;
  CriticalRef<QString> _arguments;
  CriticalRef<QSize> _viewSize;
  bool _commandUpdated;
  QImage * _outputImageA;
  QMutex * _imageMutexA;
  QImage * _outputImageB;
  QMutex * _imageMutexB;
  QSemaphore * _blockingSemaphore;
  PreviewMode _previewMode;
  int _frameSkip;
  unsigned long _frameInterval;
  int _fps;
  bool _continue;
  int _xMouse;
  int _yMouse;
  int _buttonsMouse;
  // G'MIC related members
  cimg_library::CImgList<float> _gmic_images;
  cimg_library::CImgList<char> _gmic_images_names;
  gmic * _gmic;
  bool _noFilter;
};

#endif // _FILTERTHREAD_H_
