#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QTimer>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <utility>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

struct Task {
    QString title;
    bool isCompleted;
    QDateTime dueDate;
    bool notified = false;
};

class TaskManager : public QAbstractListModel {
    Q_OBJECT

signals:
    void notifyUser(const QString &title);

public:
    enum TaskRoles {
        TitleRole = Qt::UserRole + 1,
        IsCompletedRole,
        DueDateRole
    };

    explicit TaskManager(QObject *parent = nullptr) : QAbstractListModel(parent) {
        m_filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tasks.json";
        loadTasks();

        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &TaskManager::checkDeadlines);
        m_timer->start(5000);
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_tasks.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() < 0 || index.row() >= m_tasks.size())
            return QVariant();

        const Task &task = m_tasks[index.row()];
        if (role == TitleRole) return task.title;
        if (role == IsCompletedRole) return task.isCompleted;
        if (role == DueDateRole) return task.dueDate.isValid() ? task.dueDate.toString("dd.MM.yyyy hh:mm") : "";

        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TitleRole] = "title";
        roles[IsCompletedRole] = "isCompleted";
        roles[DueDateRole] = "dueDate";
        return roles;
    }

    Q_INVOKABLE void addTask(const QString &title, int priority = 1, const QString &dueStr = "") {
        Q_UNUSED(priority);
        beginInsertRows(QModelIndex(), m_tasks.size(), m_tasks.size());
        QDateTime due = QDateTime::fromString(dueStr, "yyyy-MM-dd hh:mm");
        m_tasks.append({title, false, due, false});
        endInsertRows();
        saveTasks();
    }

    Q_INVOKABLE void removeTask(int index) {
        if (index < 0 || index >= m_tasks.size()) return;
        beginRemoveRows(QModelIndex(), index, index);
        m_tasks.removeAt(index);
        endRemoveRows();
        saveTasks();
    }

    Q_INVOKABLE void toggleCompleted(int index) {
        if (index < 0 || index >= m_tasks.size()) return;
        m_tasks[index].isCompleted = !m_tasks[index].isCompleted;
        emit dataChanged(createIndex(index, 0), createIndex(index, 0), {IsCompletedRole});
        saveTasks();
    }

private:
    QList<Task> m_tasks;
    QString m_filePath;
    QTimer *m_timer;

    void checkDeadlines() {
        QDateTime now = QDateTime::currentDateTime();
        for (auto &task : m_tasks) {
            if (!task.isCompleted && task.dueDate.isValid() && task.dueDate <= now && !task.notified) {
#ifdef Q_OS_WIN
                // Гучний наполегливий сигнал електронного будильника
                for (int i = 0; i < 4; ++i) {
                    Beep(2000, 120);
                    Sleep(60);
                    Beep(2000, 120);
                    Sleep(60);
                    Beep(2000, 120);
                    Sleep(60);
                    Beep(2400, 280);
                    Sleep(200);
                }
#endif
                emit notifyUser(task.title);
                task.notified = true;
                saveTasks();
            }
        }
    }

    void loadTasks() {
        QFile file(m_filePath);
        if (!file.open(QIODevice::ReadOnly)) return;

        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isArray()) return;

        beginResetModel();
        m_tasks.clear();
        QJsonArray array = doc.array();
        for (const QJsonValue &val : std::as_const(array)) {
            QJsonObject obj = val.toObject();
            m_tasks.append({
                obj["title"].toString(),
                obj["isCompleted"].toBool(),
                QDateTime::fromString(obj["dueDate"].toString(), Qt::ISODate),
                obj["notified"].toBool()
            });
        }
        endResetModel();
    }

    void saveTasks() {
        QDir dir(QFileInfo(m_filePath).absolutePath());
        if (!dir.exists()) dir.mkpath(".");

        QJsonArray array;
        for (const auto &task : std::as_const(m_tasks)) {
            QJsonObject obj;
            obj["title"] = task.title;
            obj["isCompleted"] = task.isCompleted;
            obj["dueDate"] = task.dueDate.toString(Qt::ISODate);
            obj["notified"] = task.notified;
            array.append(obj);
        }

        QFile file(m_filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(array).toJson());
            file.close();
        }
    }
};

#endif // TASKMANAGER_H