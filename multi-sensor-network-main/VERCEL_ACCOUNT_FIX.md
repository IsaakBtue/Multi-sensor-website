# Fix: Deployments Showing "wseng" Instead of Your Account

## Why This Happens

When Vercel shows deployments created by "wseng", it means:

1. **The project is connected to a Vercel Team/Organization** called "wseng"
2. **Not your personal account** - it's a workspace/team account
3. This is a **Vercel account/workspace setting**, not a code issue

## How to Fix It

### Option 1: Transfer Project to Your Personal Account (Recommended)

1. **Go to Vercel Dashboard:**
   - Visit [vercel.com](https://vercel.com)
   - Make sure you're logged in

2. **Find Your Project:**
   - Go to your project: `multi-sensor-network`
   - Click on **Settings**

3. **Transfer the Project:**
   - Scroll down to **"Transfer Project"** or **"General"** section
   - Look for **"Team"** or **"Organization"** dropdown
   - Change it from **"wseng"** to your **personal account** (your username)
   - Or transfer the project to your personal account

4. **Save Changes:**
   - The project will now be under your personal account
   - New deployments will show your name instead of "wseng"

### Option 2: Disconnect and Reconnect

1. **Delete the Project from Vercel:**
   - Go to project settings
   - Scroll to bottom
   - Click **"Delete Project"**

2. **Re-import from GitHub:**
   - Go to Vercel dashboard
   - Click **"Add New Project"**
   - Import from GitHub
   - **Make sure to select your personal account** (not "wseng" team)
   - Select repository: `IsaakBtue/multi-sensor-network`

3. **Deploy:**
   - Vercel will deploy under your personal account

### Option 3: Check Your Vercel Workspace

1. **Check Current Workspace:**
   - Look at the top-left of Vercel dashboard
   - You'll see a dropdown with workspace names
   - If "wseng" is selected, that's why deployments show "wseng"

2. **Switch Workspace:**
   - Click the workspace dropdown
   - Select your **personal account** (your username)
   - Or create a new personal workspace

3. **Re-deploy:**
   - Deployments will now show under your personal account

## Understanding Vercel Teams vs Personal Accounts

**Vercel Teams/Organizations:**
- Shared workspaces for teams
- Multiple members can deploy
- Shows team name (like "wseng") as creator
- Useful for collaboration

**Personal Account:**
- Your individual account
- Shows your name as creator
- Private to you

## Why "wseng" Might Be There

Possible reasons:
1. **You joined a team** called "wseng" at some point
2. **GitHub integration** was connected to a team account
3. **Project was created** while in a team workspace
4. **Organization/team** was set up for your school/work

## Quick Check

To see which account you're using:
1. Go to [vercel.com/account](https://vercel.com/account)
2. Check **"Teams"** section
3. See if "wseng" is listed there
4. Check which workspace is currently active

## After Fixing

Once you transfer to your personal account:
- ✅ Deployments will show your name
- ✅ You'll have full control
- ✅ No team restrictions
- ✅ All future deployments under your account

## If You Want to Keep "wseng" Team

If "wseng" is intentional (e.g., school/work team):
- That's fine! The deployment will work the same
- It's just showing the team name instead of your personal name
- Functionality is identical

The important thing is that **the deployment works** - the "wseng" name is just a display/ownership label.

