#include "ThumbnailProxyModel.h"

const unsigned int THUMBNAIL_SIZE = 350;

ThumbnailProxyModel::ThumbnailProxyModel(QObject* parent){

}

ThumbnailProxyModel::ThumbnailProxyModel(){

}


ThumbnailProxyModel::~ThumbnailProxyModel(){

}


void ThumbnailProxyModel::generateThumbnails(const QModelIndex& startIndex, int count){
    if(!startIndex.isValid()) return;

    const QAbstractItemModel* model = startIndex.model();
    int start = startIndex.row();
    int lastIndex = start + count;

    for(int row = start; row<lastIndex; row++){
        QString filepath = model->data(model->index(row,0),
                                       PictureModel::FilePathRole).toString();
        QPixmap pixmap(filepath);
        auto thumbnail = new QPixmap(pixmap.scaled(THUMBNAIL_SIZE,THUMBNAIL_SIZE,
                                        Qt::KeepAspectRatio,Qt::SmoothTransformation));

        mThumbnails[filepath] = thumbnail;

    }
}

void ThumbnailProxyModel::reloadThumbnails(){
    qDeleteAll(mThumbnails);
    mThumbnails.clear();
    generateThumbnails(index(0, 0), rowCount());
}

void ThumbnailProxyModel::setSourceModel(QAbstractItemModel* sourceModel){
    QIdentityProxyModel::setSourceModel(sourceModel);
    if (!sourceModel) return;

    connect(sourceModel, &QAbstractItemModel::modelReset, [this] { reloadThumbnails();});
    connect(sourceModel, &QAbstractItemModel::rowsInserted, [this]
            (const QModelIndex& parent, int first, int last)
                { generateThumbnails(index(first, 0), last - first + 1);});
}

QVariant ThumbnailProxyModel::data(const QModelIndex& index, int role) const{
    if (role != Qt::DecorationRole) return QIdentityProxyModel::data(index, role);
    QString filepath = sourceModel()->data(index, PictureModel::FilePathRole).toString();
    return *mThumbnails[filepath];
}

PictureModel* ThumbnailProxyModel::pictureModel() const{
    return static_cast<PictureModel*>(sourceModel());
}


