•••••••••# Complete DevOps CI/CD Setup Guide
**From Scratch with New Jenkins & GitHub Accounts**

---

## 📋 Prerequisites

Before starting, ensure you have:
- Docker and Docker Compose installed
- Minikube and kubectl installed
- Git installed
- A new GitHub account created
- A new Docker Hub account created

---

## STEP 1: Clean Up Existing Environment

### 1.1 Stop and Remove Old Containers
```bash
cd /home/devops/devops-project/atelier-jenkins-k8s

# Stop Jenkins container
docker stop jenkins
docker rm jenkins

# Stop Minikube
minikube stop
minikube delete
```

### 1.2 Remove Old Jenkins Data (Optional - for complete fresh start)
```bash
# Backup first if needed
sudo rm -rf ci-cd-config/jenkins_home/*
```

### 1.3 Clean Docker Images (Optional)
```bash
# Remove old images
docker image prune -a
```

---

## STEP 2: Create New GitHub Repository

### 2.1 On GitHub Website
1. Log in to your **NEW GitHub account**
2. Click the **"+"** icon (top right) → **"New repository"**
3. Repository settings:
   - **Repository name**: `devops-project` (or any name you prefer)
   - **Description**: "CI/CD Pipeline with Jenkins and Kubernetes"
   - **Visibility**: Public or Private (your choice)
   - **DO NOT** initialize with README, .gitignore, or license
4. Click **"Create repository"**

### 2.2 Note Your Repository URL
You'll see something like:
```
https://github.com/YOUR_NEW_USERNAME/devops-project.git
```
**⚠️ SAVE THIS URL - You'll need it later!**

---

## STEP 3: Initialize Git Repository

### 3.1 Configure Git (if new system)
```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

### 3.2 Initialize and Push to GitHub
```bash
cd /home/devops/devops-project/atelier-jenkins-k8s

# Remove old git history
rm -rf .git

# Initialize new repository
git init

# Add all files
git add .

# First commit
git commit -m "feat: Initial commit - DevOps CI/CD project"

# Add your NEW GitHub repository as remote
git remote add origin https://github.com/YOUR_NEW_USERNAME/devops-project.git

# Rename branch to main
git branch -M main

# Push to GitHub
git push -u origin main
```

**📝 Note:** Replace `YOUR_NEW_USERNAME` with your actual GitHub username!

**If prompted for credentials:**
- Username: Your GitHub username
- Password: Your GitHub Personal Access Token (not your password!)

**To create a Personal Access Token:**
1. GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Generate new token (classic)
3. Select scopes: `repo` (full control)
4. Generate and copy the token

---

## STEP 4: Start Jenkins with Docker Compose

### 4.1 Start Jenkins Container
```bash
cd /home/devops/devops-project/atelier-jenkins-k8s

# Start Jenkins
docker-compose -f ci-cd-config/docker-compose-jenkins.yml up -d

# Wait 30 seconds for Jenkins to start
sleep 30

# Check if Jenkins is running
docker ps | grep jenkins
```

### 4.2 Get Jenkins Initial Admin Password
```bash
# Get the password
docker exec jenkins cat /var/jenkins_home/secrets/initialAdminPassword
```

**📝 COPY THIS PASSWORD - You'll need it in the next step!**

---

## STEP 5: Configure Jenkins - First Time Setup

### 5.1 Access Jenkins Web Interface
1. Open your browser
2. Go to: **http://localhost:8080**

### 5.2 Unlock Jenkins
1. Paste the **initialAdminPassword** you copied
2. Click **"Continue"**

### 5.3 Install Plugins
1. Select **"Install suggested plugins"**
2. Wait for all plugins to install (2-5 minutes)

### 5.4 Create First Admin User
Fill in the form:
- **Username**: (e.g., `admin` or your preferred username)
- **Password**: (create a strong password)
- **Confirm password**
- **Full name**: Your name
- **Email**: Your email

**⚠️ SAVE THESE CREDENTIALS!**

Click **"Save and Continue"**

### 5.5 Instance Configuration
- Jenkins URL: `http://localhost:8080/` (keep as is)
- Click **"Save and Finish"**
- Click **"Start using Jenkins"**

---

## STEP 6: Install kubectl in Jenkins Container

### 6.1 Install kubectl
```bash
docker exec -u root jenkins sh -c "curl -LO https://dl.k8s.io/release/\$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl && chmod +x kubectl && mv kubectl /usr/local/bin/"
```

### 6.2 Verify Installation
```bash
docker exec jenkins kubectl version --client
```

---

## STEP 7: Start Minikube Cluster

### 7.1 Start Minikube
```bash
minikube start
```

Wait for Minikube to fully start (1-3 minutes)

### 7.2 Verify Cluster
```bash
kubectl get nodes
```

Expected output:
```
NAME       STATUS   ROLES           AGE   VERSION
minikube   Ready    control-plane   1m    v1.x.x
```

---

## STEP 8: Configure Kubernetes Access for Jenkins

### 8.1 Copy Kubernetes Config to Jenkins
```bash
# Create temp directory and copy config
mkdir -p /tmp/kube-config
kubectl config view --raw > /tmp/kube-config/config

# Create .kube directory in Jenkins
docker exec jenkins mkdir -p /root/.kube

# Copy config to Jenkins container
docker cp /tmp/kube-config/config jenkins:/root/.kube/config

# Set proper permissions
docker exec jenkins chmod 600 /root/.kube/config
```

### 8.2 Verify Jenkins Can Access Kubernetes
```bash
docker exec jenkins kubectl get nodes
```

Expected output: Should show the Minikube node

---

## STEP 9: Configure Docker Hub Credentials in Jenkins

### 9.1 Create Docker Hub Access Token
1. Go to **https://hub.docker.com/**
2. Log in to your Docker Hub account
3. Click your username (top right) → **"Account Settings"**
4. **"Security"** → **"New Access Token"**
5. Access token description: `jenkins-ci-cd`
6. Click **"Generate"**
7. **⚠️ COPY THE TOKEN** (you won't see it again!)

### 9.2 Add Credentials to Jenkins
1. In Jenkins web interface: **Manage Jenkins** → **Credentials**
2. Click **"System"** → **"Global credentials (unrestricted)"**
3. Click **"Add Credentials"**

Fill in:
- **Kind**: Username with password
- **Scope**: Global
- **Username**: Your Docker Hub username
- **Password**: The access token you just created
- **ID**: `dockerhub-creds`
- **Description**: `Docker Hub Credentials`

Click **"Create"**

---

## STEP 10: Update Jenkinsfile with Your Docker Hub Username

### 10.1 Edit Jenkinsfile
```bash
cd /home/devops/devops-project/atelier-jenkins-k8s
nano Jenkinsfile
```

### 10.2 Update the REGISTRY Environment Variable
Find this line (around line 5):
```groovy
REGISTRY = "midenimeed"
```

Change it to:
```groovy
REGISTRY = "YOUR_DOCKERHUB_USERNAME"
```

**Replace `YOUR_DOCKERHUB_USERNAME` with your actual Docker Hub username!**

### 10.3 Save and Exit
- Press `Ctrl + O` (save)
- Press `Enter`
- Press `Ctrl + X` (exit)

---

## STEP 11: Update Kubernetes Manifests

### 11.1 Edit Server Deployment
```bash
nano ci-cd-config/k8s-serveur-deployment.yaml
```

Find the line with image (around line 18):
```yaml
image: YOUR_REGISTRY/serveur:latest
```

Make sure it says `YOUR_REGISTRY` (this will be replaced by the pipeline)

### 11.2 Edit Client Deployment
```bash
nano ci-cd-config/k8s-client-deployment.yaml
```

Find the line with image (around line 18):
```yaml
image: YOUR_REGISTRY/client:latest
```

Make sure it says `YOUR_REGISTRY` (this will be replaced by the pipeline)

---

## STEP 12: Commit and Push Changes to GitHub

### 12.1 Add Changes
```bash
cd /home/devops/devops-project/atelier-jenkins-k8s

git add Jenkinsfile ci-cd-config/k8s-serveur-deployment.yaml ci-cd-config/k8s-client-deployment.yaml

git commit -m "chore: Update Docker Hub registry to new account"

git push origin main
```

---

## STEP 13: Create Jenkins Pipeline Job

### 13.1 Create New Pipeline
1. In Jenkins dashboard, click **"New Item"**
2. Enter item name: `devops-ci-cd-pipeline`
3. Select **"Pipeline"**
4. Click **"OK"**

### 13.2 Configure Pipeline
Scroll down to **"Pipeline"** section:

- **Definition**: Pipeline script from SCM
- **SCM**: Git
- **Repository URL**: `https://github.com/YOUR_NEW_USERNAME/devops-project.git`
  (Replace with your actual GitHub repository URL)
- **Credentials**: None (if public repo) or add GitHub credentials if private
- **Branch Specifier**: `*/main`
- **Script Path**: `Jenkinsfile`

Click **"Save"**

---

## STEP 14: Run Your First Pipeline Build

### 14.1 Trigger Build
1. In the pipeline page, click **"Build Now"**
2. Watch the build progress in **"Build History"**
3. Click on the build number (e.g., #1)
4. Click **"Console Output"** to see detailed logs

### 14.2 Expected Stages
The pipeline should execute these stages:
1. ✅ **Checkout** - Pull code from GitHub
2. ✅ **Docker Build** - Build client and server images
3. ✅ **Docker Push** - Push images to Docker Hub
4. ✅ **Deploy to K8s** - Deploy to Kubernetes

---

## STEP 15: Verify Deployment

### 15.1 Check Pods
```bash
kubectl get pods
```

Expected output:
```
NAME                                  READY   STATUS
serveur-deployment-xxxxx-xxxxx        1/1     Running
client-deployment-xxxxx-xxxxx         0/1     CrashLoopBackOff or Completed
```

**Note:** Client CrashLoopBackOff is expected - it runs once and exits

### 15.2 Check Services
```bash
kubectl get services
```

Expected output:
```
NAME              TYPE        CLUSTER-IP     PORT(S)
serveur-service   ClusterIP   10.x.x.x       6000/TCP
```

### 15.3 Check Client Logs
```bash
kubectl logs deployment/client-deployment --tail=20
```

Expected output:
```
[client] Connecting to serveur-service:6000
[client] Received: pong from server
```

### 15.4 Check Server Logs
```bash
kubectl logs deployment/serveur-deployment --tail=20
```

---

## STEP 16: Verify on Docker Hub

### 16.1 Check Your Images
1. Go to **https://hub.docker.com/**
2. Log in to your account
3. Go to **"Repositories"**
4. You should see:
   - `serveur` repository with tags (build-1, build-2, etc.)
   - `client` repository with tags

---

## 🎉 SUCCESS CHECKLIST

After completing all steps, you should have:

- ✅ New GitHub repository with your code
- ✅ Jenkins running with new admin account
- ✅ Docker Hub credentials configured
- ✅ Minikube cluster running
- ✅ Jenkins can access Kubernetes
- ✅ Pipeline job created and running successfully
- ✅ Docker images pushed to your Docker Hub
- ✅ Application deployed to Kubernetes
- ✅ Client-Server communication working

---

## 🔧 Troubleshooting

### Issue: Jenkins can't connect to GitHub
**Solution:** 
- Check your repository URL is correct
- If private repo, add GitHub credentials in Jenkins
- Use Personal Access Token, not password

### Issue: Docker push fails with authentication error
**Solution:**
- Verify Docker Hub credentials in Jenkins
- ID must be exactly: `dockerhub-creds`
- Use access token, not password

### Issue: kubectl not found in Jenkins
**Solution:**
```bash
docker exec -u root jenkins which kubectl
```
If not found, reinstall (see Step 6)

### Issue: Jenkins can't access Kubernetes
**Solution:**
```bash
# Re-copy kubeconfig
kubectl config view --raw > /tmp/kube-config/config
docker cp /tmp/kube-config/config jenkins:/root/.kube/config
docker exec jenkins kubectl get nodes
```

### Issue: Minikube not starting
**Solution:**
```bash
minikube delete
minikube start --driver=docker
```

### Issue: Pipeline fails at Docker Build
**Solution:**
- Check Dockerfiles exist in `client/` and `serveur/` directories
- Verify source code files exist (client.c, serveur.c)

---

## 📚 Important Commands Reference

### Docker
```bash
# View running containers
docker ps

# View Jenkins logs
docker logs jenkins

# Restart Jenkins
docker restart jenkins

# Stop Jenkins
docker stop jenkins
```

### Kubernetes
```bash
# View pods
kubectl get pods

# View services
kubectl get services

# View deployments
kubectl get deployments

# View pod logs
kubectl logs <pod-name>

# Delete deployment
kubectl delete deployment <deployment-name>

# Apply manifest
kubectl apply -f <filename.yaml>
```

### Git
```bash
# Check status
git status

# View commit history
git log --oneline

# Push changes
git add .
git commit -m "message"
git push origin main
```

---

## 🔄 Making Changes and Triggering New Builds

When you want to make changes:

1. Edit your code (client.c, serveur.c, etc.)
2. Commit and push:
   ```bash
   git add .
   git commit -m "feat: Your change description"
   git push origin main
   ```
3. In Jenkins, click **"Build Now"** on your pipeline
4. Jenkins will:
   - Pull latest code from GitHub
   - Build new Docker images with incremented build number
   - Push to Docker Hub
   - Deploy to Kubernetes

---

## 📝 Notes

- Jenkins workspace: `/var/jenkins_home/workspace/devops-ci-cd-pipeline`
- Build numbers increment automatically: build-1, build-2, build-3...
- Each build creates new Docker image tags
- Kubernetes automatically pulls and deploys new images

---

**Created:** December 19, 2025  
**Last Updated:** December 19, 2025

---

**🎓 You now have a complete CI/CD pipeline!**
