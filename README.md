# Atelier Jenkins + Kubernetes (C/C++)

Ce mini-projet automatise le cycle de vie d'une appli client/serveur C avec Jenkins, Docker et Kubernetes (Minikube).

## Arborescence
- `client/` : client C + Dockerfile multi-stage
- `serveur/` : serveur C + Dockerfile multi-stage
- `ci-cd-config/` : `docker-compose-jenkins.yml` + manifestes K8s
- `Jenkinsfile` : pipeline CI/CD

## Prérequis
- Git, Docker, Docker Compose
- Minikube + `kubectl` accessibles sur la machine Jenkins
- Un registre Docker (ex: Docker Hub) + identifiants Jenkins (ID `dockerhub-creds` dans l'exemple)

## Lancer Jenkins en local
```bash
cd ci-cd-config
docker-compose -f docker-compose-jenkins.yml up -d
# Mot de passe initial
docker logs jenkins | grep initialAdminPassword
```
Ouvrir http://localhost:8080, terminer l'init, installer les plugins suggérés, créer un admin.

## Configurer Jenkins
1) Créer un credential `Username with password` ID `dockerhub-creds` pointant vers votre registre.
2) Créer un job pipeline sur ce dépôt (multibranch ou pipeline libre) pointant vers `Jenkinsfile`.
3) Définir la variable `REGISTRY` dans le Jenkinsfile (ex: `monuser`).

## Pipeline (Jenkinsfile)
Etapes : Checkout ➜ Build C ➜ Docker build ➜ Docker push ➜ Deploy K8s.
Les manifestes sont templatés : `YOUR_REGISTRY/...` est remplacé par `${REGISTRY}/...:${IMAGE_TAG}` puis appliqué via `kubectl apply -f`.

## Minikube
```bash
minikube start
kubectl get nodes
```
Assurez-vous que `kubectl` est utilisable par l'agent Jenkins (kubeconfig présent).

## Validation
```bash
kubectl get pods
kubectl logs -l app=serveur
kubectl logs -l app=client
```
Le client doit afficher la connexion au service `serveur-service:6000` et le serveur doit afficher les messages reçus.

## Nettoyage
```bash
kubectl delete deployment client-deployment serveur-deployment
kubectl delete service serveur-service
```
