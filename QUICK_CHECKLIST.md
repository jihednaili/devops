# Quick Setup Checklist
**DevOps CI/CD Pipeline - New Accounts Setup**

---

## 📋 Before You Start - Information to Gather

Write down these details as you go:

```
NEW GITHUB USERNAME: _____________________
NEW GITHUB REPO URL: _____________________
NEW GITHUB PERSONAL ACCESS TOKEN: _____________________

NEW DOCKER HUB USERNAME: _____________________
NEW DOCKER HUB ACCESS TOKEN: _____________________

NEW JENKINS ADMIN USERNAME: _____________________
NEW JENKINS ADMIN PASSWORD: _____________________
JENKINS INITIAL ADMIN PASSWORD: _____________________
```

---

## ✅ Step-by-Step Checklist

### Phase 1: Cleanup (5 minutes)
- [ ] Stop Jenkins: `docker stop jenkins && docker rm jenkins`
- [ ] Delete Minikube: `minikube delete`
- [ ] Remove old git: `cd /home/devops/devops-project/atelier-jenkins-k8s && rm -rf .git`

### Phase 2: GitHub Setup (5 minutes)
- [ ] Create new GitHub account (if not done)
- [ ] Create new repository on GitHub
- [ ] Copy repository URL
- [ ] Generate Personal Access Token
- [ ] Initialize local git: `git init`
- [ ] Add remote: `git remote add origin <YOUR_URL>`
- [ ] Push code: `git add . && git commit -m "Initial commit" && git push -u origin main`

### Phase 3: Jenkins Setup (10 minutes)
- [ ] Start Jenkins: `docker-compose -f ci-cd-config/docker-compose-jenkins.yml up -d`
- [ ] Wait 30 seconds
- [ ] Get password: `docker exec jenkins cat /var/jenkins_home/secrets/initialAdminPassword`
- [ ] Open browser: http://localhost:8080
- [ ] Enter initial password
- [ ] Install suggested plugins
- [ ] Create admin user
- [ ] Save credentials!

### Phase 4: Install Tools (5 minutes)
- [ ] Install kubectl in Jenkins:
  ```bash
  docker exec -u root jenkins sh -c "curl -LO https://dl.k8s.io/release/\$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl && chmod +x kubectl && mv kubectl /usr/local/bin/"
  ```
- [ ] Verify: `docker exec jenkins kubectl version --client`

### Phase 5: Kubernetes Setup (5 minutes)
- [ ] Start Minikube: `minikube start`
- [ ] Wait for it to be Ready
- [ ] Copy config to Jenkins:
  ```bash
  mkdir -p /tmp/kube-config
  kubectl config view --raw > /tmp/kube-config/config
  docker exec jenkins mkdir -p /root/.kube
  docker cp /tmp/kube-config/config jenkins:/root/.kube/config
  ```
- [ ] Test: `docker exec jenkins kubectl get nodes`

### Phase 6: Docker Hub Setup (5 minutes)
- [ ] Create Docker Hub account (if not done)
- [ ] Generate Access Token at hub.docker.com
- [ ] In Jenkins web: Manage Jenkins → Credentials → System → Global
- [ ] Add Credentials:
  - Kind: Username with password
  - Username: Your Docker Hub username
  - Password: Your access token
  - ID: `dockerhub-creds` (exactly this!)

### Phase 7: Update Configuration Files (5 minutes)
- [ ] Edit Jenkinsfile:
  ```bash
  nano Jenkinsfile
  ```
  Change line 5: `REGISTRY = "YOUR_DOCKERHUB_USERNAME"`

- [ ] Save and exit (Ctrl+O, Enter, Ctrl+X)

- [ ] Commit changes:
  ```bash
  git add Jenkinsfile
  git commit -m "chore: Update Docker Hub username"
  git push origin main
  ```

### Phase 8: Create Jenkins Pipeline (5 minutes)
- [ ] In Jenkins: New Item
- [ ] Name: `devops-ci-cd-pipeline`
- [ ] Type: Pipeline
- [ ] Definition: Pipeline script from SCM
- [ ] SCM: Git
- [ ] Repository URL: Your GitHub URL
- [ ] Branch: `*/main`
- [ ] Script Path: `Jenkinsfile`
- [ ] Save

### Phase 9: Run Pipeline (5 minutes)
- [ ] Click "Build Now"
- [ ] Watch Console Output
- [ ] Wait for SUCCESS

### Phase 10: Verify (5 minutes)
- [ ] Check pods: `kubectl get pods`
- [ ] Check services: `kubectl get services`
- [ ] Check client logs: `kubectl logs deployment/client-deployment --tail=20`
- [ ] Check Docker Hub: Go to hub.docker.com and verify images

---

## ⏱️ Total Time: ~50 minutes

---

## 🚨 Common Issues & Quick Fixes

### Jenkins can't access GitHub
```bash
# Make sure you used Personal Access Token, not password
# GitHub Settings → Developer settings → Personal access tokens
```

### Docker push authentication failed
```bash
# In Jenkins Credentials, ID must be exactly: dockerhub-creds
# Use Docker Hub Access Token, not password
```

### kubectl not found in Jenkins
```bash
docker exec -u root jenkins sh -c "curl -LO https://dl.k8s.io/release/\$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl && chmod +x kubectl && mv kubectl /usr/local/bin/"
```

### Jenkins can't access Kubernetes
```bash
kubectl config view --raw > /tmp/kube-config/config
docker cp /tmp/kube-config/config jenkins:/root/.kube/config
docker exec jenkins kubectl get nodes
```

---

## 🎯 Success Indicators

You're done when you see:

1. ✅ Jenkins shows "Finished: SUCCESS"
2. ✅ `kubectl get pods` shows serveur running (1/1)
3. ✅ `kubectl logs deployment/client-deployment` shows "Received: pong from server"
4. ✅ Docker Hub shows your serveur and client repositories
5. ✅ GitHub shows your repository with all files

---

## 📞 Need Help?

Refer to the detailed **SETUP_GUIDE.md** for:
- Detailed explanations of each step
- Troubleshooting section
- Command reference
- Architecture explanations

---

**Good luck with your setup! 🚀**
