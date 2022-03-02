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
    role = relationship("Role")


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

    def is_user_valid(self, name, pass_hash):
        selected_user = self._session().query(User).filter(User.name == name, User.pass_hash == pass_hash).first()
        return selected_user is not None

    def is_such_user_in_base(self, name):
        selected_user = self._session().query(User).filter(User.name == name).first()
        return selected_user is not None

    def update_user(self, u):
        self._session().merge(u)
        self._session().commit()

    def add_user(self, name, password, role_id):
        new_user = User(name=str(name), password=str(password), role_id=int(role_id))
        try:
            self._session().add(new_user)
            self._session().commit()
        except IntegrityError:
            return False
        return True
