from sqlalchemy import Column, Integer, String, ForeignKey
from sqlalchemy.engine import Engine
from sqlalchemy.exc import IntegrityError
from sqlalchemy.ext.declarative import declarative_base

from sqlalchemy import create_engine, event
from sqlalchemy.orm import sessionmaker, relationship

import http_api.auth.constants as cnt
from http_api.auth.config import params

Base = declarative_base()


@event.listens_for(Engine, "connect")
def set_sqlite_pragma(dbapi_connection, connection_record):
    cursor = dbapi_connection.cursor()
    cursor.execute("PRAGMA foreign_keys=ON")
    cursor.close()


class User(Base):
    __tablename__ = cnt.USER
    id = Column(Integer, primary_key=True, unique=True)
    name = Column(String(255), nullable=False)
    password = Column(String(255), nullable=False)
    role_id = Column(Integer, ForeignKey('role.id'))
    role = relationship('Role')

    def __repr__(self):
        return '<id={}, name={}, role={}>'.format(self.id, self.name, self.role)

    @property
    def serialize(self):
        return {
            cnt.ID: self.id,
            cnt.NAME: self.name,
            cnt.ROLE: self.role.name
        }


class Role(Base):
    __tablename__ = cnt.ROLE
    id = Column(Integer, primary_key=True, unique=True)
    name = Column(String(255), nullable=False)


class DataBase:
    def __init__(self) -> None:
        self.engine = create_engine(params[cnt.SQLITE_DB_PATH])
        self.session = None

    def init(self) -> None:
        Base.metadata.create_all(self.engine)
        for key, value in params[cnt.ROLES].items():
            role = Role(id=value, name=key)
            self._session().merge(role)
        self._session().commit()

    def _session(self):
        if not self.session:
            self.session = sessionmaker(bind=self.engine)()
        return self.session

    def is_user_valid(self, name, password):
        selected_user = self._session().query(User).filter(User.name == name, User.password == password).first()
        return selected_user is not None

    def is_such_user_in_base(self, name):
        selected_user = self._session().query(User).filter(User.name == name).first()
        return selected_user is not None

    def is_such_role_in_base(self, role_id):
        selected_role = self._session().query(Role).filter(Role.id == role_id).first()
        return selected_role is not None

    def get_user_by_id(self, id: int):
        user_info = self._session().query(User).filter(User.id == id).first()
        return user_info.serialize if user_info is not None else None

    def get_users(self):
        users_info = self._session().query(User)
        users_info = [x.serialize for x in users_info.all()]
        return users_info

    def update_user(self, u):
        self._session().merge(u)
        self._session().commit()

    def add_user(self, name: str, password: str, role_id: int) -> bool:
        new_user = User(name=str(name), password=str(password), role_id=int(role_id))
        try:
            self._session().add(new_user)
            self._session().commit()
        except IntegrityError:
            return False
        return True

    def delete_user_by_id(self, id: str) -> int:
        delete_users_count = self._session().query(User).filter(User.id == id).delete()
        self._session().commit()
        return delete_users_count

    def update_user_by_id(self, id: int, name: str, password: str, role_id: str) -> bool:
        updated_users_count = self._session().query(User).filter(User.id == id).\
            update({cnt.NAME: name, cnt.PASSWORD: password, cnt.ROLE_ID: role_id})
        self._session().commit()
        return updated_users_count
