from sqlalchemy import Column, ForeignKey, Integer, TIMESTAMP, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import relationship

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

Base = declarative_base()


class User(Base):
    __tablename__ = 'user'
    id = Column(Integer, primary_key=True, unique=True)
    name = Column(String(255), nullable=False)
    pass_hash = Column(String(255), nullable=False)


class DataBase:
    def __init__(self):
        self.engine = create_engine('sqlite:///' + '../sc-machine/sc-kpm/sc-python/services/http_api/auth/data.db')
        self.session = None

    def __del__(self):
        pass

    def init(self):
        Base.metadata.create_all(self.engine)
        self._session().commit()

    def _session(self):
        if not self.session:
            self.session = sessionmaker(bind=self.engine)()
        return self.session

    # def new_expire_time(self):
    #     return datetime.date.fromtimestamp(time.time() + tornado.options.options.user_key_expire_time)

    def is_user_valid(self, name, pass_hash):
        selected_user = self._session().query(User).filter(User.name == name and User.pass_hash == pass_hash).first()
        return selected_user is not None

    def update_user(self, u):
        self._session().merge(u)
        self._session().commit()

    def add_user(self, name, pass_hash):
        new_user = User(name=str(name),
                        pass_hash=str(pass_hash))
        self._session().add(new_user)
        self._session().commit()
