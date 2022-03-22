from sqlalchemy import Column, Integer, String
from sqlalchemy.engine import Engine
from sqlalchemy.exc import IntegrityError
from sqlalchemy.ext.declarative import declarative_base

from sqlalchemy import create_engine, event
from sqlalchemy.orm import sessionmaker

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

    def __repr__(self):
        return '<id={}, name={}>'.format(self.id, self.name)

    @property
    def serialize(self):
        return {
            cnt.ID: self.id,
            cnt.NAME: self.name
        }


class DataBase:
    def __init__(self) -> None:
        self.engine = create_engine(params[cnt.SQLITE_DB_PATH])
        self.session = None

    def init(self) -> None:
        Base.metadata.create_all(self.engine)
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

    def get_users(self):
        users_info = self._session().query(User)
        users_info = [x.serialize for x in users_info.all()]
        return users_info

    def add_user(self, name: str, password: str) -> bool:
        new_user = User(name=str(name), password=str(password))
        try:
            self._session().add(new_user)
            self._session().commit()
        except IntegrityError:
            return False
        return True

    def delete_user_by_name(self, name: str) -> int:
        delete_users_count = self._session().query(User).filter(User.name == name).delete()
        self._session().commit()
        return delete_users_count

    def update_user_by_name(self, name: str, new_name: str, password: str) -> bool:
        updated_users_count = self._session().query(User).filter(User.name == name).\
            update({cnt.NAME: new_name, cnt.PASSWORD: password})
        self._session().commit()
        return updated_users_count
