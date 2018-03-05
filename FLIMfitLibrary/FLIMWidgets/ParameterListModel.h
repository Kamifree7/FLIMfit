#pragma once

#include <QAbstractItemModel>
#include <QMetaProperty>
#include "DecayModel.h"

class ParameterListItem
{
public:
   enum Type { Root, Group, Option, SubParameters, Parameter };
   
   ParameterListItem(std::shared_ptr<QDecayModel> model);
   ParameterListItem(std::shared_ptr<AbstractDecayGroup> group, int index, ParameterListItem* parent);
   ParameterListItem(std::shared_ptr<AbstractDecayGroup> group, ParameterListItem* parent);
   ParameterListItem(std::shared_ptr<AbstractDecayGroup> group, const QMetaProperty prop, ParameterListItem* parent);
   ParameterListItem(std::shared_ptr<FittingParameter> parameter, ParameterListItem* parent);

   ~ParameterListItem();

   void refresh();

   int row() const;

   Type type() { return m_type; }
   ParameterListItem* parent() { return m_parent; }
   ParameterListItem* child(int row) { return m_children.value(row); }
   int childCount() { return m_children.size(); }

   const QString& name() { return m_name; }
   std::shared_ptr<FittingParameter> parameter() { return m_parameter; }
   const QMetaProperty& property() { return m_property; }
   std::shared_ptr<AbstractDecayGroup> decayGroup() { return m_decay_group; }

   void addChild(ParameterListItem* child) { m_children.append(child); }
   
   void removeChild(int row) 
   { 
      delete m_children.at(row); 
      m_children.removeAt(row);
   }

protected:
   
   QString m_name;
   QList<ParameterListItem*> m_children;
   Type m_type;
   ParameterListItem* m_parent;

   std::shared_ptr<FittingParameter> m_parameter;
   std::shared_ptr<AbstractDecayGroup> m_decay_group;
   QMetaProperty m_property;
};

class ParameterListModel : public QAbstractItemModel
{
   Q_OBJECT

public:
   ParameterListModel(std::shared_ptr<QDecayModel> decay_model, QObject* parent = 0);
   ~ParameterListModel();

   void parseDecayModel();

   void addGroup(int group_type);
   void removeGroup(const QModelIndex index);

   QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
   QModelIndex parent(const QModelIndex & index) const;
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
   QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
   bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
   Qt::ItemFlags flags(const QModelIndex & index) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex & parent = QModelIndex()) const;

protected:

   ParameterListItem* GetItem(const QModelIndex& parent) const;

   std::shared_ptr<QDecayModel> decay_model;
   ParameterListItem *root_item;
};