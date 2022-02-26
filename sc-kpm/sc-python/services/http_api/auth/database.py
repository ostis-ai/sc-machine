from sqlalchemy import Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

import http_api.auth.constants as cnt
from http_api.auth.config import params

Base = declarative_base()


class User(Base):
    __tablename__ = cnt.USER
    id = Column(Integer, primary_key=True, unique=True)
    name = Column(String(255), nullable=False)
    pass_hash = Column(String(255), nullable=False)


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

    def is_user_valid(self, name, pass_hash):
        selected_user = self._session().query(User).filter(User.name == name, User.pass_hash == pass_hash).first()
        return selected_user is not None

    def update_user(self, u):
        self._session().merge(u)
        self._session().commit()

    def add_user(self, name, pass_hash):
        new_user = User(name=str(name),
                        pass_hash=str(pass_hash))
        self._session().add(new_user)
        self._session().commit()
