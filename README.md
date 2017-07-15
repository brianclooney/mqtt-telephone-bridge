sudo docker-compose -f docker/dev/docker-compose.yml up -d

./docker/dev/build.sh

sudo docker-compose -f docker/dev/docker-compose.yml restart

sudo docker-compose -f docker/dev/docker-compose.yml logs -f

