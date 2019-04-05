/** -*- mode: c++ ; c-basic-offset: 2 -*-
 * @file   View2DWidget.h
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
 */
#ifndef _IMAGEVIEW_H_
#define _IMAGEVIEW_H_

#include <QMutex>
#include <QTime>
#include <QWidget>
#include "KeypointList.h"

class QPaintEvent;

class ImageView : public QWidget {
  Q_OBJECT

public:
  ImageView(QWidget * parent = 0);
  inline QImage & image();
  inline QMutex & imageMutex();
  void setImageSize(int width, int height);
  void setBackgroundColor(QColor);
  void setKeypoints(const KeypointList & keypoints);
  KeypointList keypoints() const;
  QRect imagePosition();

public slots:
  void zoomOriginal();
  void zoomFitBest();
  void checkSize();

protected:
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseDoubleClickEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void resizeEvent(QResizeEvent *);
  void keyPressEvent(QKeyEvent *);
  void closeEvent(QCloseEvent *);

signals:
  void mousePress(QMouseEvent *);
  void mouseMove(QMouseEvent *);
  void mouseDoubleClick(QMouseEvent *);
  void aboutToClose();
  void spaceBarPressed();
  void escapePressed();
  void keypointPositionsChanged();
  void resized(QSize);

private:
  QMouseEvent mapMousePositionToImage(QMouseEvent * e);
  QImage _image;
  QMutex _imageMutex;
  QRect _imagePosition;
  double _scaleFactor;
  bool _zoomOriginal;
  QColor _backgroundColor;
  KeypointList _keypoints;
  int _movedKeypointIndex;
  QTime _keypointTimestamp;
  static int roundedDistance(const QPoint & p1, const QPoint & p2);
  int keypointUnderMouse(const QPoint & p);
  void paintKeypoints(QPainter & painter);
  QPoint keypointToPointInWidget(const KeypointList::Keypoint & kp) const;
  QPoint keypointToVisiblePointInWidget(const KeypointList::Keypoint & kp) const;
  QPointF pointInWidgetToKeypointPosition(const QPoint & p) const;
};

QImage & ImageView::image()
{
  return _image;
}

QMutex & ImageView::imageMutex()
{
  return _imageMutex;
}

#endif
