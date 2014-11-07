wget http://download.redis.io/releases/redis-2.8.4.tar.gz
tar xzf redis-2.8.4.tar.gz
cd redis-2.8.4
make
sudo make install

echo "Create upstart configuration file for redis server"
sudo bash -c 'cat <<EOF > /etc/init/redis-server.conf
start on runlevel [2345]
stop on runlevel [016]

respawn
script
        exec redis-server
end script
EOF'

sudo start redis-server
